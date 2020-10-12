#pragma once

#include "Scanner.h"
#include <StringArray.h>
#include <Tokenizer.h>

typedef enum {
  NO_MODIFIER
 ,ZEROORONE     // ?
 ,ZEROORMANY    // *
 ,ONEORMANY     // +
} SymbolModifier;

typedef enum {
  PLATFORM_X86 = 0
 ,PLATFORM_X64 = 1
} Platform;

class ParserTables {
public:
  virtual int          getAction(           UINT state, UINT input   ) const = 0; // > 0:shift, <=0:reduce, _ParserError:Error
  virtual int          getSuccessor(        UINT state, UINT nt      ) const = 0;
  virtual UINT         getProductionLength( UINT prod                ) const = 0;
  virtual UINT         getLeftSymbol(       UINT prod                ) const = 0;
  virtual const TCHAR *getSymbolName(       UINT symbol              ) const = 0;
  const   TCHAR       *getLeftSymbolName(   UINT prod                ) const {
    return getSymbolName(getLeftSymbol(prod));
  }
          String       getRightString(      UINT prod                ) const;
  virtual void         getRightSide(        UINT prod, UINT *dst     ) const = 0;
  virtual UINT         getTerminalCount()                              const = 0;
  virtual UINT         getSymbolCount()                                const = 0;
  virtual UINT         getProductionCount()                            const = 0;
  virtual UINT         getStateCount()                                 const = 0;
  virtual UINT         getLegalInputCount(  UINT state               ) const = 0;
  virtual void         getLegalInputs(      UINT state, UINT *symbols) const = 0;
  virtual UINT         getTableByteCount(   Platform platform        ) const = 0;

  virtual ~ParserTables() {
  }
};

#define _ParserError 0xffff

template<typename Type, typename SuccIndexType> class ParserTablesTemplate : public ParserTables {
private:
  const UINT          *m_actionCode;
  const BYTE          *m_compressedLasets;
  const Type          *m_uncompressedActions;
  const SuccIndexType *m_successorsIndex;
  const Type          *m_stateSuccessors;
  const BYTE          *m_productionLength;
  const Type          *m_leftSide;
  const Type          *m_rightSideTable;
  const char          *m_nameString;
  const USHORT         m_terminalCount;
  const USHORT         m_symbolCount;
  const USHORT         m_productionCount;
  const USHORT         m_stateCount;
  const UINT           m_tableByteCountx86,m_tableByteCountx64;
  mutable const Type **m_rightSides;
  mutable StringArray  m_symbolNameTable;

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v>>3]&(1<<(v&7))) != 0;
  }

  int findElementUncompressed(const Type *uaArray, UINT symbol) const {
    for(int l = 0, r = *(uaArray++) - 1; l <= r;) { // binary search
      const int m = (l+r)>>1;
      const int cmp = uaArray[m<<1] - (int)symbol;
      if(cmp < 0) {
        l = m + 1;
      } else if(cmp > 0) {
        r = m - 1;
      } else {
        return uaArray[(m<<1)|1];
      }
    }
    return _ParserError;
  }

  void buildSymbolNameTable() const {
    String tmp = m_nameString;
    m_symbolNameTable.setCapacity(getSymbolCount());
    for(Tokenizer tok(tmp, _T(" ")); tok.hasNext();) {
      m_symbolNameTable.add(tok.next());
    }
  }
  inline bool isSingleItemActionState(UINT code) const {
    return (code & 0x8000) == 0;
  }
  inline const BYTE *getCompressedLaset(UINT code) const {
    return m_compressedLasets + (code&0x7fff);
  }
  inline int getCompressedAction(UINT code) const {
    return ((int)code<<1) >> 17; // signed right shift!!
  }
  inline int getActionCompressedSingleItem(UINT code, UINT token) const {
    return ((code&0x7fff) == token) ? getCompressedAction(code) : _ParserError;
  }
  inline int getActionCompressedMultiItem(UINT code, UINT token) const {
    return contains(getCompressedLaset(code), token) ? getCompressedAction(code) : _ParserError;
  }
  inline int findActionCompressed(UINT code, UINT token) const {
    return isSingleItemActionState(code)
         ? getActionCompressedSingleItem(code, token)
         : getActionCompressedMultiItem( code, token);
  }
  inline void getLegalInputsCompressedSingleItem(UINT code, UINT *symbols) const {
    *symbols = (code&0x7fff);
  }

  void getLegalInputsCompressedMultiItem(UINT code, UINT *symbols) const {
    const BYTE *set = getCompressedLaset(code);
    for(UINT token = 0; token < m_terminalCount; token++) {
      if(contains(set, token)) {
        *(symbols++) = token;
      }
    }
  }

  UINT getLegalInputCountCompressedMultiItem(UINT code) const {
    const BYTE *set = getCompressedLaset(code);
    UINT        sum = 0;
    for(UINT byteCount = (m_terminalCount-1)/8 + 1; byteCount--;) {
      for(BYTE b = *(set++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }

  void getLegalInputsCompressed(UINT code, UINT *symbols) const {
    if(isSingleItemActionState(code)) {
      getLegalInputsCompressedSingleItem(code, symbols);
    } else {
      getLegalInputsCompressedMultiItem(code, symbols);
    }
  }

  UINT getLegalInputCountCompressed(UINT code) const {
    return isSingleItemActionState(code)
         ? 1
         : getLegalInputCountCompressedMultiItem(code);
  }

  void getLegalInputsUncompressed(const Type *uaArray, UINT *symbols) const {
    for(int n = *(uaArray++); n--; uaArray+=2) {
      *(symbols++) = *uaArray;
    }
  }

  inline UINT getLegalInputCountUncompressed(const Type *uaArray) const {
    return uaArray[0];
  }

  const Type **getRightSides() const {
    if(m_rightSides == nullptr) {
      m_rightSides = new const Type*[m_productionCount]; TRACE_NEW(m_rightSides);
      for(UINT p = 0, index = 0; p < m_productionCount; p++) {
        const UINT prodLen = m_productionLength[p];
        m_rightSides[p] = prodLen ? (m_rightSideTable + index) : NULL;
        index += prodLen;
      }
    }
    return m_rightSides;
  }

  inline bool isCompressedState(UINT state) const {
    return (m_actionCode[state] & 0x80000000) != 0;
  }

  inline UINT getCompressedActionCode(UINT state) const {
    return m_actionCode[state] & 0x7fffffff;
  }
  inline const Type *getUncompressedActionList(UINT state) const {
    return m_uncompressedActions + m_actionCode[state];
  }
public:

#pragma warning(push)
//#pragma warning(disable:4311 4302)

   // token is terminal. return > 0:shift, <=0:reduce, _ParserError:Error
  int getAction(UINT state, UINT token)   const override {
    return isCompressedState(state)
         ? findActionCompressed(   getCompressedActionCode(  state), token)
         : findElementUncompressed(getUncompressedActionList(state), token);
  }

  // nt is nonterminal
  int getSuccessor(UINT state, UINT nt)   const override {
    return findElementUncompressed(m_stateSuccessors + m_successorsIndex[state], nt);
  };

  UINT getProductionLength(UINT prod)     const override {
    return m_productionLength[prod];
  }

  UINT getLeftSymbol(UINT prod)           const override {
    return m_leftSide[prod];
  }

  const TCHAR *getSymbolName(UINT symbol) const override {
    if(m_symbolNameTable.size() == 0) {
      buildSymbolNameTable();
    }
    return m_symbolNameTable[symbol].cstr();
  }

  void getRightSide(UINT prod, UINT *dst) const override {
    UINT l = getProductionLength(prod);
    if(l == 0) {
      return;
    }
    const Type *rightSide = getRightSides()[prod];
    while(l--) {
      *(dst++) = *(rightSide++);
    }
  }

  UINT getTerminalCount()                 const override {
    return m_terminalCount;
  }

  UINT getSymbolCount()                   const override {
    return m_symbolCount;
  }

  UINT getProductionCount()               const override {
    return m_productionCount;
  }

  UINT getStateCount()                    const override {
    return m_stateCount;
  }
  UINT getLegalInputCount(UINT state)     const override {
    return isCompressedState(state)
         ? getLegalInputCountCompressed(  getCompressedActionCode(  state))
         : getLegalInputCountUncompressed(getUncompressedActionList(state));
  }

  void getLegalInputs(UINT state, UINT *symbols) const override {
    if(isCompressedState(state)) {
      getLegalInputsCompressed(  getCompressedActionCode(  state), symbols);
    } else {
      getLegalInputsUncompressed(getUncompressedActionList(state), symbols);
    }
  }
  UINT getTableByteCount(Platform platform)      const override {
    return (platform==PLATFORM_X86) ? m_tableByteCountx86 : m_tableByteCountx64;
  }
#pragma warning(pop)

  ParserTablesTemplate(const UINT          *actionCode
                      ,const BYTE          *compressedLasets
                      ,const Type          *uncompressedActions
                      ,const SuccIndexType *successorsIndex
                      ,const Type          *stateSuccessors
                      ,const BYTE          *productionLength
                      ,const Type          *leftSide
                      ,const Type          *rightSideTable
                      ,const char          *nameString
                      ,USHORT               terminalCount
                      ,USHORT               symbolCount
                      ,USHORT               productionCount
                      ,USHORT               stateCount
                      ,UINT                 tableByteCountx86
                      ,UINT                 tableByteCountx64
                      )
   :m_actionCode         ( actionCode         )
   ,m_compressedLasets   ( compressedLasets   )
   ,m_uncompressedActions( uncompressedActions)
   ,m_successorsIndex    ( successorsIndex    )
   ,m_stateSuccessors    ( stateSuccessors    )
   ,m_productionLength   ( productionLength   )
   ,m_leftSide           ( leftSide           )
   ,m_rightSideTable     ( rightSideTable     )
   ,m_nameString         ( nameString         )
   ,m_terminalCount      ( terminalCount      )
   ,m_symbolCount        ( symbolCount        )
   ,m_productionCount    ( productionCount    )
   ,m_stateCount         ( stateCount         )
   ,m_tableByteCountx86  (tableByteCountx86   )
   ,m_tableByteCountx64  (tableByteCountx64   )
  {
    m_rightSides      = nullptr;
  }
  ~ParserTablesTemplate() override {
    SAFEDELETEARRAY(m_rightSides);
  }
};

class ParserStackElement {
public:
  USHORT         m_state;
  USHORT         m_symbol;
  SourcePosition m_pos;
  inline ParserStackElement()
    : m_state( 0)
    , m_symbol(0)
  {
  }
  inline ParserStackElement(USHORT state, USHORT symbol, const SourcePosition &pos)
    : m_state( state )
    , m_symbol(symbol)
    , m_pos(   pos   )
  {
  }
};

class LRparser {
private:
  // Current parserstate
  UINT                m_state;
  // Current inputsymbol
  int                 m_input;
  UINT                m_stackSize;
  ParserStackElement *m_parserStack;
  // Topelement of stack = parserStack[m_stackTop-1]
  UINT                m_stackTop;
  // Dont give errormessage when > 0. Decremented on every parsecycle
  // and set to m_cascadecount when PAaction return _ParserError
  UINT                m_suppressError;
  // Suppress the next m_cascadecount parsererrors
  UINT                m_cascadeCount;
  // Maximal number of errors before terminate parse.
  UINT                m_maxErrorCount;
  // Count parsererrors
  UINT                m_errorCount;
  // Have we finished the parse
  bool                m_done;
  // Current lexeme
  TCHAR              *m_text;
  // Length of current lexeme
  int                 m_textLength;
  // Current SourcePosition
  SourcePosition      m_pos;
  // Length of current reduceproduction
  UINT                m_productionLength;
  // If true call debug on each parsecycle
  bool                m_debug;
  // Generated by parsergen.exe
  const ParserTables &m_tables;
  // Lexical scanner. generated by lexgen.exe
  Scanner            *m_scanner;

  LRparser(const LRparser &src);                     // Not defined. Class not cloneable
  LRparser &operator=(const LRparser &rhs);          // Not defined. Class not cloneable

  void parserStackCreate(UINT stackSize);
  void parserStackDestroy();
  void stackOverflow();

  inline void parserStackInit() {
    m_stackTop = 0;
  }

  inline void parserStackShift(USHORT state, USHORT symbol, const SourcePosition &pos) {
    if(m_stackTop < m_stackSize - 1) {
      m_parserStack[m_stackTop++] = ParserStackElement(state, symbol, pos);
    } else {
      stackOverflow();
    }
  }

  inline const ParserStackElement &getParserStackTop(int fromTop = 0) const {
    return m_parserStack[m_stackTop-1-fromTop];
  }

  inline void parserStackPop(UINT count) {
    m_stackTop -= count;
  }

  inline void parserStackRestore(UINT newTop) {
    m_stackTop = newTop;
  }

  bool recover();
  void initialize();
  void dumpState();
protected:
  // Called for each reduction in the parse
  virtual int reduceAction(UINT prod) {
    return 0;
  }
  // Called before the first parsecycle
  virtual void userStackInit()                   = 0;
  // Called when LRparser shift in inputtoken
  virtual void userStackShiftSymbol(UINT symbol) = 0;
  // Pop count symbols from userstack
  virtual void userStackPopSymbols( UINT count ) = 0;
  // Push($$) to userstack. called at the end of each reduction
  virtual void userStackShiftLeftSide()          = 0;
  // $$ = $1
  virtual void defaultReduce(       UINT prod  ) = 0;
public:
  LRparser(const ParserTables &tables, Scanner *lex = NULL, UINT stackSize = 256);
  virtual ~LRparser();

  inline int input() const {
    return m_input;
  }

  inline UINT state() const {
    return m_state;
  }

  inline bool stackEmpty() const {
    return m_stackTop == 0;
  }

  inline UINT getStackHeight() const {
    return m_stackTop;
  }

  inline UINT getStackSize() const {
    return m_stackSize;
  }

  void setStackSize(UINT newSize);

  inline const ParserStackElement &getStackElement(UINT index) const {
    return m_parserStack[index];
  }

  inline void setCascadeCount(UINT value) {
    m_cascadeCount = value;
  }

  inline UINT getCascadeCount() const {
    return m_cascadeCount;
  }

  inline void setMaxErrorCount(UINT value) {
    m_maxErrorCount = value;
  }

  inline UINT getMaxErrorCount() const {
    return m_maxErrorCount;
  }

  inline UINT getProductionLength(UINT prod) const {
    return m_tables.getProductionLength(prod);
  }

  inline const TCHAR *getSymbolName(UINT symbol) const {
    return m_tables.getSymbolName(symbol);
  }

  inline const ParserTables &getParserTables() const {
    return m_tables;
  }

  int getNextAction() const;

  // Set new scanner, return old scanner
  Scanner *setScanner(Scanner *lex);

  inline Scanner *getScanner() {
    return m_scanner;
  }
  // Return 0 on ok. < 0 on error.
  int parseBegin();
  // Return 0 on continue, nonzero will terminate parse.
  int parseStep();
  // Return 0 on accept. nonzero on error
  int parse();

  inline void setDebug(bool newValue) {
    m_debug = newValue;
  }

  inline bool done() const {
    return m_done;
  }

  inline bool accept() const {
    return m_errorCount == 0;
  }

  inline const TCHAR *getText() const {
    return m_text;
  }

  inline int getTextLength() const {
    return m_textLength;
  }

  inline int getLineNumber() const {
    return m_pos.getLineNumber();
  }

  // Return current sourceposition
  SourcePosition getPos() const;
  // Return sourceposition of symbol number i in current production. i = [1..prodlen]
  const SourcePosition &getPos(int i) const;
  void error(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // Called on every step of the parse if m_debug is true
  void debug(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  // Errors can be caught by usersupplied error-handler
  virtual void verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  virtual void vdebug(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
};
