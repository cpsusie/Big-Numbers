#pragma once

#include "Scanner.h"

typedef enum {
  NO_MODIFIER
 ,ZEROORONE     // ?
 ,ZEROORMANY    // *
 ,ONEORMANY     // +
} SymbolModifier;

class ParserTables {
public:
  virtual int          getAction(           UINT state, int input) const = 0; // > 0:shift, <=0:reduce, _ParserError:Error
  virtual int          getSuccessor(        UINT state, int nt   ) const = 0;
  virtual UINT         getProductionLength( UINT prod  ) const = 0;
  virtual UINT         getLeftSymbol(       UINT prod  ) const = 0;
  virtual const TCHAR *getSymbolName(       UINT symbol) const = 0;
  const   TCHAR       *getLeftSymbolName(   UINT prod  ) const {
    return getSymbolName(getLeftSymbol(prod));
  }
          String       getRightString(      UINT prod  ) const;
  virtual void         getRightSide(        UINT prod, UINT *dst) const = 0;
  virtual UINT         getTerminalCount()   const = 0;
  virtual UINT         getSymbolCount()     const = 0;
  virtual UINT         getProductionCount() const = 0;
  virtual UINT         getStateCount()      const = 0;
  virtual UINT         getLegalInputCount(  UINT state ) const = 0;
  virtual void         getLegalInputs(      UINT state, UINT *symbols) const = 0;
  virtual ~ParserTables() {
  }
};

#define _ParserError 0xffff

template <class Type> class ParserTablesTemplate : public ParserTables {
private:
  const BYTE          *m_compressedSet;
  const BYTE          *m_compressedLasets;
  const void  * const *m_action;
  const Type  * const *m_successor;
  const unsigned char *m_productionLength;
  const Type          *m_leftSide;
  const Type          *m_rightSideTable;
  const TCHAR        **m_symbolName;
  const USHORT         m_terminalCount;
  const USHORT         m_symbolCount;
  const USHORT         m_productionCount;
  const USHORT         m_stateCount;
  mutable const Type **m_rightSides;

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v/8]&(1<<(v%8))) != 0;
  }

  int findElementUncompressed(const Type *state, int token) const {
    int l = 0;
    int r = *(state++)-1;
    while(l <= r) { // binary search
      const int m = (l+r)>>1;
      const int cmp = state[m<<1] - token;
      if(cmp < 0) {
        l = m + 1;
      } else if(cmp > 0) {
        r = m - 1;
      } else {
        return state[(m<<1)|1];
      }
    }
    return _ParserError;
  }

  inline bool isSingleItemActionState(ULONG statev) const {
    return (statev & 0x8000) == 0;
  }
  inline const BYTE *getCompressedLaset(ULONG statev) const {
    return m_compressedLasets + (statev&0x7fff);
  }
  inline int getCompressedAction(ULONG statev) const {
    return (long)statev >> 16;
  }

  inline int getActionCompressedSingleItem(ULONG statev, int token) const {
    return ((statev&0x7fff) == (ULONG)token) ? getCompressedAction(statev) : _ParserError;
  }

  inline int getActionCompressedMultiItem(ULONG statev, int token) const {
    return contains(getCompressedLaset(statev), token) ? getCompressedAction(statev) : _ParserError;
  }
  inline int findActionCompressed(ULONG statev, int token) const {
    return isSingleItemActionState(statev)
         ? getActionCompressedSingleItem(statev, token)
         : getActionCompressedMultiItem( statev, token);
  }

  inline void getLegalInputsCompressedSingleItem(ULONG statev, UINT *symbols) const {
    *symbols = (statev&0x7fff);
  }

  void getLegalInputsCompressedMultiItem(ULONG statev, UINT *symbols) const {
    const BYTE *set = getCompressedLaset(statev);
    for(int token = 0; token < m_terminalCount; token++) {
      if(contains(set, token)) {
        *(symbols++) = token;
      }
    }
  }

  int getLegalInputCountCompressedMultiItem(ULONG statev) const {
    const BYTE *set = getCompressedLaset(statev);
    int         sum = 0;
    for(int byteCount = (m_terminalCount-1)/8 + 1; byteCount--;) {
      for(BYTE b = *(set++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }

  void getLegalInputsCompressed(ULONG statev, UINT *symbols) const {
    if(isSingleItemActionState(statev)) {
      getLegalInputsCompressedSingleItem(statev, symbols);
    } else {
      getLegalInputsCompressedMultiItem(statev, symbols);
    }
  }

  int getLegalInputCountCompressed(ULONG statev) const {
    return isSingleItemActionState(statev)
         ? 1
         : getLegalInputCountCompressedMultiItem(statev);
  }

  void getLegalInputsUncompressed(const Type *state, UINT *symbols) const {
    for(int n = *(state++); n--; state+=2) {
      *(symbols++) = *state;
    }
  }

  inline int getLegalInputCountUncompressed(const Type *state) const {
    return state[0];
  }

  const Type **getRightSides() const {
    if(m_rightSides == NULL) {
      m_rightSides = new const Type*[m_productionCount]; TRACE_NEW(m_rightSides);
      int index = 0;;
      for(int p = 0; p < m_productionCount; p++) {
        const UINT prodLen = m_productionLength[p];
        m_rightSides[p] = prodLen ? (m_rightSideTable + index) : NULL;
        index += prodLen;
      }
    }
    return m_rightSides;
  }

  inline bool isCompressedState(UINT state) const {
    return contains(m_compressedSet, state);
  }

public:
#pragma warning(push)
#pragma warning(disable:4311 4302)

   // token is terminal. return > 0:shift, <=0:reduce, _ParserError:Error
  inline int getAction(UINT state, int token) const {
    return isCompressedState(state)
         ? findActionCompressed(   (ULONG      )m_action[state], token)
         : findElementUncompressed((const Type*)m_action[state], token);
  }

  // nt is nonterminal
  inline int getSuccessor(UINT state, int nt) const {
    return findElementUncompressed(m_successor[state], nt);
  };

  inline UINT getProductionLength(UINT prod) const {
    return m_productionLength[prod];
  }

  inline UINT getLeftSymbol(UINT prod) const {
    return m_leftSide[prod];
  }

  inline const TCHAR *getSymbolName(UINT symbol) const {
    return m_symbolName[symbol];
  }

  void getRightSide(UINT prod, UINT *dst) const {
    UINT l = getProductionLength(prod);
    if(l == 0) {
      return;
    }
    const Type *rightSide = getRightSides()[prod];
    while(l--) {
      *(dst++) = *(rightSide++);
    }
  }

  inline UINT getTerminalCount() const {
    return m_terminalCount;
  }

  inline UINT getSymbolCount() const {
    return m_symbolCount;
  }

  inline UINT getProductionCount() const {
    return m_productionCount;
  }

  inline UINT getStateCount() const {
    return m_stateCount;
  }
  inline UINT getLegalInputCount(UINT state) const {
    return isCompressedState(state)
         ? getLegalInputCountCompressed(  (ULONG      )(m_action[state]))
         : getLegalInputCountUncompressed((const Type*)(m_action[state]));
  }

  void getLegalInputs(UINT state, UINT *symbols) const {
    if(isCompressedState(state)) {
      getLegalInputsCompressed(  (ULONG      )(m_action[state]), symbols);
    } else {
      getLegalInputsUncompressed((const Type*)(m_action[state]), symbols);
    }
  }
#pragma warning(pop)

  ParserTablesTemplate(const BYTE           *compressedSet
                     , const BYTE           *compressedLasets
                     , const void * const   *action
                     , const Type * const   *successor
                     , const unsigned char  *productionLength
                     , const Type           *leftSide
                     , const Type           *rightSideTable
                     , const TCHAR         **symbolName
                     , USHORT                terminalCount
                     , USHORT                symbolCount
                     , USHORT                productionCount
                     , USHORT                stateCount)
   :m_compressedSet    ( compressedSet   )
   ,m_compressedLasets ( compressedLasets)
   ,m_action           ( action          )
   ,m_successor        ( successor       )
   ,m_productionLength ( productionLength)
   ,m_leftSide         ( leftSide        )
   ,m_rightSideTable   ( rightSideTable  )
   ,m_symbolName       ( symbolName      )
   ,m_terminalCount    ( terminalCount   )
   ,m_symbolCount      ( symbolCount     )
   ,m_productionCount  ( productionCount )
   ,m_stateCount       ( stateCount      )
  {
    m_rightSides = NULL;
  }
  ~ParserTablesTemplate() {
    SAFEDELETEARRAY(m_rightSides);
  }
};

typedef ParserTablesTemplate<char>  ParserCharTables;
typedef ParserTablesTemplate<short> ParserShortTables;

class ParserStackElement {
public:
  USHORT m_state;
  USHORT m_symbol;
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
  ~LRparser();

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
  // Return 0 on continue, nonzero 0 terminate parse.
  int parseStep();
  // Return 0 on accept. nonzero 0 on error
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
