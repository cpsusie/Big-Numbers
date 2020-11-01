#pragma once

#include "Scanner.h"
#include "StringArray.h"
#include "Tokenizer.h"

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
  virtual int          getAction(           UINT state, UINT term    ) const = 0; // > 0:shift, <=0:reduce, _ParserError:Error
  virtual UINT         getSuccessor(        UINT state, UINT nt      ) const = 0;
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
  const TerminalType        *m_termListTable;
  const ActionType          *m_actionListTable;
  const BYTE                *m_termSetTable;
  const UINT                *m_successorCode;
  const NTIndexType         *m_NTindexListTable;
  const StateType           *m_stateListTable;
  const BYTE                *m_productionLength;
  const NTIndexType         *m_leftSideTable;
  const SymbolType          *m_rightSideTable;
  const char                *m_nameString;
  const UINT                 m_tableByteCountx86, m_tableByteCountx64;
  mutable const SymbolType **m_rightSides;
  mutable StringArray        m_symbolNameTable;

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v>>3]&(1<<(v&7))) != 0;
  }

  // Binary search.
  // Assume a[0] contains the number of elements, n, in the array, followed by n distinct elements, in increasing order
  // Return index [0..n-1] of the searched element, if it exist, or -1 if not found
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

// ----------------------------- successor functions ---------------------------------------
  static inline bool isCompressedSuccCode(UINT code) {
    return (code & 0x00010000) != 0;
  }

  inline UINT getCompressedSuccessor(UINT code, UINT nt) const {
    assert((nt - terminalCount) == (code & 0xffff));
    return code >> 17;
  }
  inline const NTIndexType *getNTindexList(UINT code) const {
    assert(!isCompressedSuccCode(code));
    return m_NTindexListTable + (code & 0xffff);
  }
  inline UINT getUncompressedSuccessor(UINT code, UINT nt) const {
    const int index = findElement(getNTindexList(code), nt - terminalCount);
    assert(index >= 0);
    return m_stateListTable[(code >> 17) + index];
  }

// ----------------------------------- action functions ---------------------------------
  static constexpr unsigned char CodeTermList  = 0;
  static constexpr unsigned char CodeSplitNode = 1;
  static constexpr unsigned char CodeOneItem   = 2;
  static constexpr unsigned char CodeTermSet   = 3;

  static inline BYTE getActMethodCode(UINT code) {
    return (code >> 15) & 3;
  }
  inline const TerminalType *getTermList(UINT code) const {
    return m_termListTable + (code & 0x7fff);
  }
  inline const BYTE         *getTermSet(UINT code) const {
    return m_termSetTable  + (code & 0x7fff);
  }
  inline UINT left( UINT code) const { return m_actionCode[code & 0x7fff]; }
  inline UINT right(UINT code) const { return m_actionCode[code >> 17   ]; }

// --------------------------- getLegalInputCount -------------------------------------

  static inline UINT getLegalInputCountTermList(const TerminalType *termList) {
    return termList[0];
  }
  inline UINT getLegalInputCountSplitNode(UINT code) const {
    return getLegalInputCountFromCode(left(code)) + getLegalInputCountFromCode(right(code));
  }
  static UINT getLegalInputCountTermSet(const BYTE *termSet) {
    UINT sum = 0;
    constexpr UINT termSetByteCount = (terminalCount - 1) / 8 + 1;
    for(const BYTE *endp = termSet + termSetByteCount; termSet < endp;) {
      for(BYTE b = *(termSet++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }
  inline UINT getLegalInputCountFromCode(UINT code) const {
    switch(getActMethodCode(code)) {
    case CodeTermList : return getLegalInputCountTermList(getTermList(code));
    case CodeSplitNode: return getLegalInputCountSplitNode(           code );
    case CodeOneItem  : return 1;
    case CodeTermSet  : return getLegalInputCountTermSet( getTermSet( code));
    }
    return 0;
  }

// --------------------------- getLegalInputs -------------------------------------

  static inline UINT getLegalInputsTermList(const TerminalType *termList, UINT *symbols) {
    const UINT n = *(termList++);
    for(const TerminalType *endp = termList+n; termList < endp;) {
      *(symbols++) = *(termList++);
    }
    return n;
  }
  inline UINT getLegalInputsSplitNode(UINT code, UINT *symbols) const {
    const UINT n = getLegalInputsFromCode(left(code), symbols);
    return n + getLegalInputsFromCode(right(code), symbols + n);
  }
  static UINT getLegalInputsTermSet(const BYTE *termSet, UINT *symbols) {
    UINT count = 0;
    for(UINT term = 0; term < terminalCount; term++) {
      if(contains(termSet, term)) {
        *(symbols++) = term;
        count++;
      }
    }
    return count;
  }
  inline UINT getLegalInputsFromCode(UINT code, UINT *symbols) const {
    switch(getActMethodCode(code)) {
    case CodeTermList : return getLegalInputsTermList(getTermList(code), symbols);
    case CodeSplitNode: return getLegalInputsSplitNode(           code , symbols);
    case CodeOneItem  : *symbols = (code & 0x7fff); return 1;
    case CodeTermSet  : return getLegalInputsTermSet( getTermSet( code), symbols);
    }
    return 0;
  }

// --------------------------- getAction -------------------------------------
  inline int getActionTermList(UINT code, UINT term) const {
    const int index = findElement(getTermList(code), term);
    return (index < 0) ? _ParserError : m_actionListTable[(code >> 17) + index];
  }
  inline int getActionSplitNode(UINT code, UINT term) const {
    const int a = getActionFromCode(left(code), term);
    return (a != _ParserError) ? a : getActionFromCode(right(code), term);
  }
  inline int getActionFromCode(UINT code, UINT term) const {
    switch(getActMethodCode(code)) {
    case CodeTermList : return getActionTermList(                 code, term);
    case CodeSplitNode: return getActionSplitNode(                code, term);
    case CodeOneItem  : return ((code & 0x7fff) == term)        ? ((signed int)code >> 17) : _ParserError;
    case CodeTermSet  : return contains(getTermSet(code), term) ? ((signed int)code >> 17) : _ParserError;
    }
    return 0;
  }

// ------------------------------------ misc---------------------------------

  void buildSymbolNameTable() const {
    String tmp = m_nameString;
    m_symbolNameTable.setCapacity(getSymbolCount());
    for(Tokenizer tok(tmp, _T(" ")); tok.hasNext();) {
      m_symbolNameTable.add(tok.next());
    }
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
public:
#pragma warning(push)

  // term is a terminal-symbol.
  // Return action
  //   action >  0 : Shift to state = action
  //   action <  0 : Reduce by production p = -action;
  //   action == 0 : Accept, ie. reduce by production 0
  //   _ParserError: Unexpected term
  int getAction(UINT state, UINT term)           const override {
    assert(state < stateCount);
    return getActionFromCode(m_actionCode[state], term);
  }

  // nt is nonterminal
  UINT getSuccessor(UINT state, UINT nt)         const override {
    assert(state < stateCount);
    const UINT code = m_successorCode[state];
    return isCompressedSuccCode(code) ? getCompressedSuccessor(code, nt) : getUncompressedSuccessor(code, nt);
  }

  UINT getLegalInputCount(UINT state)            const override {
    assert(state < stateCount);
    return getLegalInputCountFromCode(m_actionCode[state]);
  }
  void getLegalInputs(UINT state, UINT *symbols) const override {
    assert(state < stateCount);
    getLegalInputsFromCode(m_actionCode[state], symbols);
  }

  UINT getProductionLength(  UINT prod  )        const override {
    assert(prod < productionCount);
    return m_productionLength[prod];
  }
  UINT getLeftSymbol(        UINT prod  )        const override {
    assert(prod < productionCount);
    return terminalCount + m_leftSideTable[prod];
  }
  const TCHAR *getSymbolName(UINT symbol)        const override {
    assert(symbol < symbolCount);
    if(m_symbolNameTable.size() == 0) {
      buildSymbolNameTable();
    }
    return m_symbolNameTable[symbol].cstr();
  }
  void getRightSide(UINT prod, UINT *dst)        const override {
    assert(prod < productionCount);
    UINT l = getProductionLength(prod);
    if(l == 0) {
      return;
    }
    const SymbolType *rightSide = getRightSides()[prod];
    while(l--) {
      *(dst++) = *(rightSide++);
    }
  }

  UINT getTerminalCount()                        const override { return terminalCount;   }
  UINT getSymbolCount()                          const override { return symbolCount;     }
  UINT getProductionCount()                      const override { return productionCount; }
  UINT getStateCount()                           const override { return stateCount;      }
  UINT getTableByteCount(Platform platform)      const override {
    return (platform==PLATFORM_X86) ? m_tableByteCountx86 : m_tableByteCountx64;
  }
#pragma warning(pop)

  ParserTablesTemplate(const UINT          *actionCode
                      ,const TerminalType  *termListTable
                      ,const ActionType    *actionListTable
                      ,const BYTE          *termSetTable
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
   ,m_termListTable          ( termListTable          )
   ,m_actionListTable        ( actionListTable        )
   ,m_termSetTable           ( termSetTable           )
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
  LRparser(const ParserTables &tables, Scanner *lex = nullptr, UINT stackSize = 256);
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
