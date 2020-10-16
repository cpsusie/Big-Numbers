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

template<UINT     terminalCount
        ,UINT     symbolCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,typename TerminalType     /* unsigned, values:[0..terminalCount           [ */
        ,typename NTIndexType      /* unsigned, values:[0..nonterminalCount        [ */
        ,typename SymbolType       /* unsigned, values:[0..symbolCount             [ */
        ,typename ActionType       /* signed  , values:[-maxproduction..stateCount [ */
        ,typename StateType>       /* unsigned, values:[0..stateCount              [ */
class ParserTablesTemplate : public ParserTables {
private:
  const UINT                *m_actionCode;
  const BYTE                *m_compressedLAsets;
  const TerminalType        *m_termListTable;
  const ActionType          *m_actionListTable;
  const UINT                *m_successorCode;
  const NTIndexType         *m_NTindexListTable;
  const StateType           *m_stateListTable;
  const BYTE                *m_productionLength;
  const NTIndexType         *m_leftSideTable;
  const SymbolType          *m_rightSideTable;
  const char                *m_nameString;
  const UINT                 m_tableByteCountx86,m_tableByteCountx64;
  mutable const SymbolType **m_rightSides;
  mutable StringArray        m_symbolNameTable;

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v>>3]&(1<<(v&7))) != 0;
  }

  // Binary search.
  // Assume a[0] contains the number of elements, n, in the array, followed by n distinct elements, in increasing order
  // return index of the search element, if it exist, or -1 if not found
  template<typename T> static int findElement(const T *a, int symbol) {
    const int n = *(a++);
    for(int l = 0, r = n - 1; l <= r;) { // binary search
      const int m   = (l+r)>>1;
      const int cmp = (int)a[m] - (int)symbol;
      if(cmp < 0) {
        l = m + 1;
      } else if(cmp > 0) {
        r = m - 1;
      } else {
        return m;
      }
    }
    return -1;
  }

  void buildSymbolNameTable() const {
    String tmp = m_nameString;
    m_symbolNameTable.setCapacity(getSymbolCount());
    for(Tokenizer tok(tmp, _T(" ")); tok.hasNext();) {
      m_symbolNameTable.add(tok.next());
    }
  }
  static inline bool isSingleItemActionState(UINT code) {
    return (code & 0x8000) == 0;
  }
  inline const BYTE *getCompressedLAset(UINT code) const {
    return m_compressedLAsets + (code&0x7fff);
  }
  static inline int getCompressedAction(signed int code) { // must be signed int
    return code >> 17;
  }
  static inline int getActionCompressedSingleItem(UINT code, UINT token) {
    return ((code&0x7fff) == token) ? getCompressedAction(code) : _ParserError;
  }
  inline int getActionCompressedMultiItem(UINT code, UINT token) const {
    return contains(getCompressedLAset(code), token) ? getCompressedAction(code) : _ParserError;
  }
  inline int findActionCompressed(UINT code, UINT token) const {
    return isSingleItemActionState(code)
         ? getActionCompressedSingleItem(code, token)
         : getActionCompressedMultiItem( code, token);
  }
  static inline void getLegalInputsCompressedSingleItem(UINT code, UINT *symbols) {
    *symbols = (code&0x7fff);
  }

  void getLegalInputsCompressedMultiItem(UINT code, UINT *symbols) const {
    const BYTE *set = getCompressedLAset(code);
    for(UINT token = 0; token < terminalCount; token++) {
      if(contains(set, token)) {
        *(symbols++) = token;
      }
    }
  }

  UINT getLegalInputCountCompressedMultiItem(UINT code) const {
    const BYTE *set = getCompressedLAset(code);
    UINT        sum = 0;
    for(UINT byteCount = (terminalCount-1)/8 + 1; byteCount--;) {
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

  static inline void getLegalInputsUncompressed(const TerminalType *a, UINT *symbols) {
    const UINT n = *(a++);
    for(const TerminalType *last = a+n; a < last;) {
      *(symbols++) = *(a++);
    }
  }

  static inline UINT getLegalInputCountUncompressed(const TerminalType *a) {
    return a[0];
  }

  inline const TerminalType *getTerminalArray(UINT code) const {
    return m_termListTable + (code & 0xffff);
  }

  inline UINT getLegalInputCountUncompressed(UINT code) const {
    return getLegalInputCountUncompressed(getTerminalArray(code));
  }
  inline void getLegalInputsUncompressed(UINT code, UINT *symbols) const {
    getLegalInputsUncompressed(getTerminalArray(code), symbols);
  }
  inline const int findActionUncompressed(UINT code) const {
    const int index = findElement(getTerminalArray(code));
    return (index >= 0) ? m_uncompActionTable[(code>>17)+index] : _ParserError;
  }

  const SymbolType **getRightSides() const {
    if(m_rightSides == nullptr) {
      m_rightSides = new const SymbolType*[productionCount]; TRACE_NEW(m_rightSides);
      for(UINT p = 0, index = 0; p < productionCount; p++) {
        const UINT prodLen = m_productionLength[p];
        m_rightSides[p] = prodLen ? (m_rightSideTable + index) : nullptr;
        index += prodLen;
      }
    }
    return m_rightSides;
  }

  static inline bool isCompressedCode(UINT code) {
    return (code & 0x00010000) != 0;
  }
  inline bool isCompressedState(UINT state) const {
    return isCompressedCode(m_actionCode[state]);
  }

public:

#pragma warning(push)

   // token is terminal. return > 0:shift, <=0:reduce, _ParserError:Error
  int getAction(UINT state, UINT token)          const override {
    const UINT code = m_actionCode[state];
    if(isCompressedCode(code)) {
      return findActionCompressed(code, token);
    } else {
      const int index = findElement(m_termListTable + (code & 0xffff), token);
      return (index < 0) ? _ParserError : m_actionListTable[(code >> 17) + index];
    }
  }

  // nt is nonterminal
  int getSuccessor(UINT state, UINT nt)          const override {
    const UINT code  = m_successorCode[state];
    if(isCompressedCode(code)) {
      assert((nt - terminalCount) == (code & 0x7fff));
      return code >> 17;
    } else {
      const int index = findElement(m_NTindexListTable + (code & 0xffff), nt - terminalCount);
      assert(index >= 0);
      return m_stateListTable[(code >> 17) + index];
    }
  }

  UINT getProductionLength(  UINT prod  )        const override {
    return m_productionLength[prod];
  }
  UINT getLeftSymbol(        UINT prod  )        const override {
    return terminalCount + m_leftSideTable[prod];
  }
  const TCHAR *getSymbolName(UINT symbol)        const override {
    if(m_symbolNameTable.size() == 0) {
      buildSymbolNameTable();
    }
    return m_symbolNameTable[symbol].cstr();
  }
  void getRightSide(UINT prod, UINT *dst)        const override {
    UINT l = getProductionLength(prod);
    if(l == 0) {
      return;
    }
    const SymbolType *rightSide = getRightSides()[prod];
    while(l--) {
      *(dst++) = *(rightSide++);
    }
  }

  UINT getTerminalCount()                        const override {
    return terminalCount;
  }
  UINT getSymbolCount()                          const override {
    return symbolCount;
  }
  UINT getProductionCount()                      const override {
    return productionCount;
  }
  UINT getStateCount()                           const override {
    return stateCount;
  }
  UINT getLegalInputCount(UINT state)            const override {
    const UINT code = m_actionCode[state];
    return isCompressedCode(code)
         ? getLegalInputCountCompressed(  code)
         : getLegalInputCountUncompressed(code);
  }
  void getLegalInputs(UINT state, UINT *symbols) const override {
    const UINT code = m_actionCode[state];
    if(isCompressedCode(code)) {
      getLegalInputsCompressed(  code, symbols);
    } else {
      getLegalInputsUncompressed(code, symbols);
    }
  }
  UINT getTableByteCount(Platform platform)      const override {
    return (platform==PLATFORM_X86) ? m_tableByteCountx86 : m_tableByteCountx64;
  }
#pragma warning(pop)

  ParserTablesTemplate(const UINT          *actionCode
                      ,const BYTE          *compressedLAsets
                      ,const TerminalType  *termListTable
                      ,const ActionType    *actionListTable
                      ,const UINT          *successorCode
                      ,const NTIndexType   *NTindexListTable
                      ,const StateType     *stateListTable
                      ,const BYTE          *productionLength
                      ,const NTIndexType   *leftSideTable
                      ,const SymbolType    *rightSideTable
                      ,const char          *nameString
                      ,UINT                 tableByteCountx86
                      ,UINT                 tableByteCountx64
                      )
   :m_actionCode             ( actionCode             )
   ,m_compressedLAsets       ( compressedLAsets       )
   ,m_termListTable          ( termListTable          )
   ,m_actionListTable        ( actionListTable        )
   ,m_successorCode          ( successorCode          )
   ,m_NTindexListTable       ( NTindexListTable       )
   ,m_stateListTable         ( stateListTable         )
   ,m_productionLength       ( productionLength       )
   ,m_leftSideTable          ( leftSideTable          )
   ,m_rightSideTable         ( rightSideTable         )
   ,m_nameString             ( nameString             )
   ,m_tableByteCountx86      ( tableByteCountx86      )
   ,m_tableByteCountx64      ( tableByteCountx64      )
  {
    m_rightSides = nullptr;
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
