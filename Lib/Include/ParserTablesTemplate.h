#pragma once

#include "StringArray.h"
#include "AbstractParserTables.h"

template<UINT     terminalCount
        ,UINT     symbolCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,typename TerminalType     /* unsigned, values:[0..terminalCount           [ */
        ,typename NTIndexType      /* unsigned, values:[0..nonterminalCount        [ */
        ,typename SymbolType       /* unsigned, values:[0..symbolCount             [ */
        ,typename ActionType       /* signed  , values:[-maxproduction..stateCount [ */
        ,typename StateType>       /* unsigned, values:[0..stateCount              [ */
class ParserTablesTemplate : public AbstractParserTables {
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

private:

  static inline bool contains(const BYTE *bitset, UINT v) {
    return (bitset[v>>3]&(1<<(v&7))) != 0;
  }
  static inline UINT getBitsetSize(const BYTE *bitset, UINT bytes) {
    UINT sum = 0;
    for(const BYTE *endp = bitset + bytes; bitset < endp;) {
      for(BYTE b = *(bitset++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }

  static constexpr UINT termSetByteCount = (terminalCount - 1) / 8 + 1;

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

  static inline CompressionMethod getCompressionCode(UINT code) {
    return (code >> 15) & 3;
  }

// ----------------------------- successor functions ---------------------------------------
  static inline UINT getSuccessorImmediate(UINT code, UINT nterm) {
    assert((nterm - terminalCount) == (code & 0x7fff));
    return code >> 17;
  }
  inline const NTIndexType *getNTindexList(UINT code) const {
    return m_NTindexListTable + (code & 0x7fff);
  }
  inline UINT getSuccessorBinSearch(       UINT code, UINT nterm) const {
    const int index = findElement(getNTindexList(code), nterm - terminalCount);
    assert(index >= 0);
    return m_stateListTable[(code >> 17) + index];
  }

  inline UINT getSuccesorFromCode(         UINT code, UINT nterm) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getSuccessorBinSearch(code, nterm);
    case CompCodeImmediate : return getSuccessorImmediate(code, nterm);
    default                : throwInvalidArgumentException(__TFUNCTION__,_T("Invalid compressionCode:%#08x"), code);
    }
    return 0;
  }

  inline UINT getLegalNTermCountBinSearch( UINT code) const {
    const NTIndexType *indexList = getNTindexList(code);
    return indexList[0];
  }
  inline UINT getLegalNTermCountFromCode(  UINT code) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getLegalNTermCountBinSearch(code);
    case CompCodeImmediate : return 1;
    default                : return 0;
    }
  }

  inline UINT getLegalNTermsBinSearch(     UINT code, UINT *symbols) const {
    const NTIndexType *NTindexList = getNTindexList(code);
    const UINT         n           = *(NTindexList++);
    for(const NTIndexType *endp = NTindexList+n; NTindexList < endp;) {
      *(symbols++) = *(NTindexList++) + terminalCount;
    }
    return n;
  }

  inline UINT getLegalNTermsImmediate(     UINT code, UINT *symbols) const {
    *symbols = (code & 0x7fff) + terminalCount;
    return 1;
  }

  inline UINT getLegalNTermsFromCode(      UINT code, UINT *symbols) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getLegalNTermsBinSearch(code, symbols);
    case CompCodeImmediate : return getLegalNTermsImmediate(code, symbols);
    default                : return 0;
    }
  }

// ----------------------------------- action functions ---------------------------------
  inline const TerminalType *getTermList(UINT code) const {
    return m_termListTable + (code & 0x7fff);
  }
  inline const  BYTE        *getTermSet( UINT code) const {
    return m_termSetTable  + (code & 0x7fff);
  }
  inline        UINT left(  UINT code) const { return m_actionCode[code >> 17   ]; }
  inline        UINT right( UINT code) const { return m_actionCode[code & 0x7fff]; }

// --------------------------- getLegalInputCount -------------------------------------

  inline        UINT getLegalInputCountBinSearch( UINT code) const {
    const TerminalType *termList = getTermList(code);
    return termList[0];
  }
  inline        UINT getLegalInputCountSplitNode( UINT code) const {
    return getLegalInputCountFromCode(left(code)) + getLegalInputCountFromCode(right(code));
  }
  static inline UINT getLegalInputCountImmediate( UINT code)       {
    return 1;
  }
  inline        UINT getLegalInputCountBitset(    UINT code) const {
    return getBitsetSize(getTermSet(code), termSetByteCount);
  }
  inline        UINT getLegalInputCountFromCode(  UINT code) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getLegalInputCountBinSearch(code);
    case CompCodeSplitNode : return getLegalInputCountSplitNode(code);
    case CompCodeImmediate : return getLegalInputCountImmediate(code);
    case CompCodeBitset    : return getLegalInputCountBitset(   code);
    default                : __assume(0);
    }
  }

// --------------------------- getLegalInputs -------------------------------------

  inline        UINT getLegalInputsBinSearch(   UINT code, UINT *symbols) const {
    const TerminalType *termList = getTermList(code);
    const UINT          n        = *(termList++);
    for(const TerminalType *endp = termList+n; termList < endp;) {
      *(symbols++) = *(termList++);
    }
    return n;
  }
  inline        UINT getLegalInputsSplitNode( UINT code, UINT *symbols) const {
    const UINT n = getLegalInputsFromCode(left(code), symbols);
    return n + getLegalInputsFromCode(right(code), symbols + n);
  }
  static inline UINT getLegalInputsImmediate( UINT code, UINT *symbols)       {
    *symbols = (code & 0x7fff);
    return 1;
  }
  UINT               getLegalInputsBitset(    UINT code, UINT *symbols) const {
    UINT           *symp             = symbols;
    const     BYTE *termSet          = getTermSet(code);
    for(const BYTE *bp = termSet, *endp = bp + termSetByteCount; bp < endp; bp++) {
      if(*bp) {
        UINT bitIndex = ((UINT)(bp - termSet)) << 3;
        for(BYTE b = *bp; b; bitIndex++, b >>= 1) {
          if(b & 1) *(symp++) = bitIndex;
        }
      }
    }
    return (UINT)(symp - symbols);
  }
  inline        UINT getLegalInputsFromCode(  UINT code, UINT *symbols) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getLegalInputsBinSearch(code, symbols);
    case CompCodeSplitNode : return getLegalInputsSplitNode(code, symbols);
    case CompCodeImmediate : return getLegalInputsImmediate(code, symbols);
    case CompCodeBitset    : return getLegalInputsBitset(   code, symbols);
    default                : __assume(0);
    }
  }

// --------------------------- getAction -------------------------------------
  inline        int getActionBinSearch( UINT code, UINT term) const {
    const int index = findElement(getTermList(code), term);
    return (index < 0) ? _ParserError : m_actionListTable[(code >> 17) + index];
  }
  inline        int getActionSplitNode( UINT code, UINT term) const {
    const int a = getActionFromCode(left(code), term);
    return (a != _ParserError) ? a : getActionFromCode(right(code), term);
  }
  static inline int getActionImmediate( UINT code, UINT term)       {
    return ((code & 0x7fff) == term)        ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionBitSet(    UINT code, UINT term) const {
    return contains(getTermSet(code), term) ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionFromCode( UINT code, UINT term) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getActionBinSearch(code, term);
    case CompCodeSplitNode : return getActionSplitNode(code, term);
    case CompCodeImmediate : return getActionImmediate(code, term);
    case CompCodeBitset    : return getActionBitSet(   code, term);
    default                : __assume(0);
    }
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
  int getAction(UINT state, UINT term)               const override {
    assert(state < stateCount);
    return getActionFromCode(m_actionCode[state], term);
  }

  // nterm is nonterminal
  UINT getSuccessor(      UINT state, UINT nterm)    const override {
    assert(state < stateCount);
    return getSuccesorFromCode(m_successorCode[state], nterm);
  }

  UINT getLegalInputCount(UINT state               ) const override {
    assert(state < stateCount);
    return getLegalInputCountFromCode(m_actionCode[state]);
  }
  void getLegalInputs(    UINT state, UINT *symbols) const override {
    assert(state < stateCount);
    getLegalInputsFromCode(m_actionCode[state], symbols);
  }
  UINT getLegalNTermCount(UINT state               ) const override {
    assert(state < stateCount);
    return getLegalNTermCountFromCode(m_successorCode[state]);
  }
  void getLegalNTerms(    UINT state, UINT *symbols) const override {
    assert(state < stateCount);
    getLegalNTermsFromCode(m_successorCode[state], symbols);
  }

  UINT getProductionLength(UINT prod               ) const override {
    assert(prod < productionCount);
    return m_productionLength[prod];
  }
  UINT getLeftSymbol(      UINT prod               ) const override {
    assert(prod < productionCount);
    return terminalCount + m_leftSideTable[prod];
  }
  const String &getSymbolName(UINT symbol)           const override {
    assert(symbol < symbolCount);
    if(m_symbolNameTable.size() == 0) {
      buildSymbolNameTable();
    }
    return m_symbolNameTable[symbol];
  }
  void getRightSide(UINT prod, UINT *dst)            const override {
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

  UINT getTermCount()                            const override { return terminalCount;   }
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
