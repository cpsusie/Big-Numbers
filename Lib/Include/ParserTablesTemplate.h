#pragma once

#include "StringArray.h"
#include "AbstractParserTables.h"

template<UINT     symbolCount
        ,UINT     termCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,UINT     tableByteCountx86
        ,UINT     tableByteCountx64
        ,typename SymbolType        /* unsigned, values:[0..symbolCount [ */
        ,typename NTIndexType>      /* unsigned, values:[0..ntermCount  [ */
class GenereratedTablesCommon : public AbstractParserTables {
private:
  const BYTE                *m_prodLengthArray;
  const NTIndexType         *m_leftSideArray;
  const SymbolType          *m_rightSideTable;
  const char                *m_nameString;
  mutable const SymbolType **m_rightSides;
  mutable StringArray        m_symbolNameTable;

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
        const UINT prodLen = m_prodLengthArray[p];
        m_rightSides[p] = prodLen ? (m_rightSideTable + index) : nullptr;
        index += prodLen;
      }
    }
    return m_rightSides;
  }

protected:
  GenereratedTablesCommon(const BYTE          *prodLengthArray
                         ,const NTIndexType   *leftSideArray
                         ,const SymbolType    *rightSideTable
                         ,const char          *nameString
                         )
   :m_prodLengthArray        ( prodLengthArray        )
   ,m_leftSideArray          ( leftSideArray          )
   ,m_rightSideTable         ( rightSideTable         )
   ,m_nameString             ( nameString             )
  {
    m_rightSides = nullptr;
  }
  ~GenereratedTablesCommon() override {
    SAFEDELETEARRAY(m_rightSides);
  }

  // ---------------------------------------- helper functions for ordered arrays -----------------------------------------

  template<typename T> static inline UINT getArraySize(const T *a) {
    const int n = *(a++);
    return n;
  }
  // Assume a[0] contains the number of elements, n, in the array, followed by n distinct elements, in increasing order
  // Copy all n elements to dst[0..n-1]
  // Return n
  template<typename T, typename D> static UINT getAllArrayElements(const T *a, D *dst) {
    const int n = *(a++);
    for(const T *endp = a+n; a < endp;) {
      *(dst++) = (D)(*(a++));
    }
    return n;
  }
  // Binary search.
  // Assume a[0] contains the number of elements, n, in the array, followed by n distinct elements, in increasing order
  // Return index [0..n-1] of the searched element, if it exist, or -1 if not found
  template<typename T> static int findArrayIndex(const T *a, int v) {
    const int n = *(a++);
    for(int l = 0, r = n - 1; l <= r;) { // binary search
      const int m   = (l+r)>>1;
      const int cmp = (int)a[m] - v;
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

  // ---------------------------------------- helper functions for bitsets -----------------------------------------

  static inline UINT getSizeofBitSet(UINT capacity) {
    return (capacity - 1) / 8 + 1;
  }

  // bitset is a pointer to first BYTE in bitset, capacity = max value in bitset + 1. values in bitset=[0..capacity-1]
  // Return number of 1-bits in bitset
  static inline UINT getBitSetSize(const BYTE *bitset, UINT capacity) {
    const UINT bytes = getSizeofBitSet(capacity);
    UINT       sum   = 0;
    for(const BYTE *endp = bitset + bytes; bitset < endp;) {
      for(BYTE b = *(bitset++); b; b &= (b-1)) {
        sum++;
      }
    }
    return sum;
  }
  // bitset is a pointer to first BYTE in bitset, capacity = max value in bitset + 1. values in bitset=[0..capacity-1]
  // Copy all integer-values with a 1-bit to dst[0..n-1], where n=getBitSetSize(bitset,bytes)
  // Return n
  template<typename T> static UINT getAllBitSetElements(const BYTE *bitset, UINT capacity, T *dst) {
    const UINT bytes = getSizeofBitSet(capacity);
    T         *dstp  = dst;
    for(const BYTE *bp = bitset, *endp = bp + bytes; bp < endp; bp++) {
      if(*bp) {
        UINT bitIndex = ((UINT)(bp - bitset)) << 3;
        for(BYTE b = *bp; b; bitIndex++, b >>= 1) {
          if(b & 1) *(dstp++) = (T)bitIndex;
        }
      }
    }
    return (UINT)(dstp - dst);
  }
  // bitset is a pointer to first BYTE in bitset, capacity = max value in bitset + 1. values in bitset=[0..capacity-1]
  // Return true, if v is present in bitset, ie, has a 1-bit at position indexed by v
  static inline bool bitsetContains(const BYTE *bitset, UINT capacity, UINT v) {
    return (v < capacity) && (bitset[v>>3]&(1<<(v&7))) != 0;
  }

  static inline CompressionMethod getCompressionCode(UINT code) {
    return (code >> 15) & 3;
  }
  static inline UINT leftChild(  const UINT *codeArray, UINT code) { return codeArray[code >> 17   ]; }
  static inline UINT rightChild( const UINT *codeArray, UINT code) { return codeArray[code & 0x7fff]; }

public:
  UINT getProductionLength(   UINT prod)            const final {
    assert(prod < productionCount);
    return m_prodLengthArray[prod];
  }
  UINT getLeftSymbol(         UINT prod)            const final {
    assert(prod < productionCount);
    return termCount + m_leftSideArray[prod];
  }
  const String &getSymbolName(UINT symbol)          const final {
    assert(symbol < symbolCount);
    if(m_symbolNameTable.size() == 0) {
      buildSymbolNameTable();
    }
    return m_symbolNameTable[symbol];
  }
  void getRightSide(          UINT prod, UINT *dst) const final {
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

  UINT getSymbolCount()                             const final { return symbolCount;     }
  UINT getTermCount()                               const final { return termCount;       }
  UINT getProductionCount()                         const final { return productionCount; }
  UINT getStateCount()                              const final { return stateCount;      }
  UINT getTableByteCount(Platform platform)         const final {
    return (platform==PLATFORM_X86) ? tableByteCountx86 : tableByteCountx64;
  }
};


// ----------------------------------------------------------------------------------------------

template<UINT     symbolCount
        ,UINT     termCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,UINT     tableByteCountx86
        ,UINT     tableByteCountx64
        ,UINT     termBitSetCapacity
        ,typename SymbolType       /* unsigned, values:[0..symbolCount             [ */
        ,typename TerminalType     /* unsigned, values:[0..termCount               [ */
        ,typename NTIndexType      /* unsigned, values:[0..ntermCount              [ */
        ,typename ActionType       /* signed  , values:[-maxproduction..stateCount [ */
        ,typename StateType>       /* unsigned, values:[0..stateCount              [ */
class ParserTablesTemplate
: public GenereratedTablesCommon<symbolCount
                                ,termCount
                                ,productionCount
                                ,stateCount
                                ,tableByteCountx86
                                ,tableByteCountx64
                                ,SymbolType
                                ,NTIndexType>
{
private:
  const UINT                *m_actionCodeArray;
  const TerminalType        *m_termArrayTable;
  const ActionType          *m_actionArrayTable;
  const BYTE                *m_termBitSetTable;
  const UINT                *m_successorCodeArray;
  const NTIndexType         *m_ntIndexArrayTable;
  const StateType           *m_newStateArrayTable;

// ----------------------------- successor functions ---------------------------------------
  inline const NTIndexType *getNTIndexArray(     UINT code) const {
    return m_ntIndexArrayTable + (code & 0x7fff);
  }
  inline        UINT getSuccessorBinSearch(      UINT code, UINT nterm) const {
    const int index = findArrayIndex(getNTIndexArray(code), nterm - termCount);
    return (index >= 0) ? m_newStateArrayTable[(code >> 17) + index] : _ParserError;
  }
  static inline UINT getSuccessorImmediate(      UINT code, UINT nterm) {
    return ((nterm - termCount) == (code & 0x7fff)) ? (code >> 17) : _ParserError;
  }
  inline        UINT getSuccessorFromCode(       UINT code, UINT nterm) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getSuccessorBinSearch(code, nterm);
    case CompCodeImmediate : return getSuccessorImmediate(code, nterm);
    default                : throwInvalidArgumentException(__TFUNCTION__,_T("Invalid compressionCode:%#08x"), code);
    }
    return 0;
  }

// ----------------------------------- action functions ---------------------------------
  inline const TerminalType *getTermArray(       UINT code) const {
    return m_termArrayTable  + (code & 0x7fff);
  }
  inline const  BYTE        *getTermBitSet(      UINT code) const {
    return m_termBitSetTable + (code & 0x7fff);
  }

// --------------------------- getAction -------------------------------------
  inline        int getActionBinSearch(          UINT code, UINT term) const {
    const int index = findArrayIndex(getTermArray(code), term);
    return (index >= 0) ? m_actionArrayTable[(code >> 17) + index] : _ParserError;
  }
  inline        int getActionSplitNode(          UINT code, UINT term) const {
    const int a = getActionFromCode(leftChild(m_actionCodeArray, code), term);
    return (a != _ParserError) ? a : getActionFromCode(rightChild(m_actionCodeArray, code), term);
  }
  static inline int getActionImmediate(          UINT code, UINT term)       {
    return ((code & 0x7fff) == term)        ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionBitSet(             UINT code, UINT term) const {
    return bitsetContains(getTermBitSet(code), termBitSetCapacity, term) ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionFromCode(           UINT code, UINT term) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getActionBinSearch(code, term);
    case CompCodeSplitNode : return getActionSplitNode(code, term);
    case CompCodeImmediate : return getActionImmediate(code, term);
    case CompCodeBitSet    : return getActionBitSet(   code, term);
    default                : __assume(0);
    }
  }

public:
#pragma warning(push)

  // term is a terminal-symbol.
  // Return action
  //   action >  0 : Shift to state = action
  //   action <  0 : Reduce by production p = -action;
  //   action == 0 : Accept, ie. reduce by production 0
  //   _ParserError: Unexpected term
  int getAction(          UINT state, UINT term     ) const final {
    assert(state < stateCount);
    return getActionFromCode(m_actionCodeArray[state], term);
  }

  // nterm is nonterminal
  UINT getSuccessor(      UINT state, UINT nterm    ) const final {
    assert(state < stateCount);
    assert((nterm >= termCount) && (nterm < symbolCount));
    return getSuccessorFromCode(m_successorCodeArray[state], nterm);
  }

#pragma warning(pop)

  ParserTablesTemplate(const BYTE          *prodLengthArray
                      ,const NTIndexType   *leftSideArray
                      ,const SymbolType    *rightSideTable
                      ,const char          *nameString
                      ,const UINT          *actionCodeArray
                      ,const TerminalType  *termArrayTable
                      ,const ActionType    *actionArrayTable
                      ,const BYTE          *termBitSetTable
                      ,const UINT          *successorCodeArray
                      ,const NTIndexType   *ntIndexArrayTable
                      ,const StateType     *newStateArrayTable
                      )
    :GenereratedTablesCommon(prodLengthArray  , leftSideArray
                            ,rightSideTable   , nameString
                            )
    ,m_actionCodeArray    (actionCodeArray    )
    ,m_termArrayTable     (termArrayTable     )
    ,m_actionArrayTable   (actionArrayTable   )
    ,m_termBitSetTable    (termBitSetTable    )
    ,m_successorCodeArray (successorCodeArray )
    ,m_ntIndexArrayTable  (ntIndexArrayTable  )
    ,m_newStateArrayTable (newStateArrayTable )
  {
  }
};



// -------------------------------------------------------------------------------------------------------------------


template<UINT     symbolCount
        ,UINT     termCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,UINT     tableByteCountx86
        ,UINT     tableByteCountx64
        ,UINT     termBitSetCapacity
        ,UINT     stateBitSetCapacity
        ,typename SymbolType       /* unsigned, values:[0..symbolCount             [ */
        ,typename TerminalType     /* unsigned, values:[0..termCount               [ */
        ,typename NTIndexType      /* unsigned, values:[0..ntermCount              [ */
        ,typename ActionType       /* signed  , values:[-maxproduction..stateCount [ */
        ,typename StateType>       /* unsigned, values:[0..stateCount              [ */
class ParserTablesTemplateTransSucc
: public GenereratedTablesCommon<symbolCount
                                ,termCount
                                ,productionCount
                                ,stateCount
                                ,tableByteCountx86
                                ,tableByteCountx64
                                ,SymbolType
                                ,NTIndexType>
{
private:
  const UINT                *m_actionCodeArray;
  const TerminalType        *m_termArrayTable;
  const ActionType          *m_actionArrayTable;
  const BYTE                *m_termBitSetTable;
  const UINT                *m_successorCodeArray;
  const StateType           *m_stateArrayTable;
  const StateType           *m_newStateArrayTable;
  const BYTE                *m_stateBitSetTable;

// ----------------------------- successor functions ---------------------------------------
  inline const  StateType   *getStateArray(      UINT code) const {
    return m_stateArrayTable + (code & 0x7fff);
  }
  inline const  BYTE        *getStateBitSet(     UINT code) const {
    return m_stateBitSetTable  + (code & 0x7fff);
  }
  inline        UINT getSuccessorBinSearch(      UINT code, UINT state) const {
    const int index = findArrayIndex(getStateArray(code), state);
    return (index >= 0) ? m_newStateArrayTable[(code >> 17) + index] : _ParserError;
  }
  inline        UINT getSuccessorSplitNode(      UINT code, UINT state) const {
    const UINT a = getSuccessorFromCode(leftChild(m_successorCodeArray, code), state);
    return (a != _ParserError) ? a : getSuccessorFromCode(rightChild(m_successorCodeArray, code), state);
  }
  static inline UINT getSuccessorImmediate(      UINT code, UINT state) {
    const UINT fromState = code & 0x7fff;
    return ((fromState == _NoFromStateCheck) || (state == fromState)) ? (code >> 17) : _ParserError;
  }
  inline        UINT getSuccessorBitSet(         UINT code, UINT state) const {
    return bitsetContains(getStateBitSet(code), stateBitSetCapacity, state) ? (code >> 17) : _ParserError;
  }

  inline        UINT getSuccessorFromCode(       UINT code, UINT state) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getSuccessorBinSearch(code, state);
    case CompCodeSplitNode : return getSuccessorSplitNode(code, state);
    case CompCodeImmediate : return getSuccessorImmediate(code, state);
    case CompCodeBitSet    : return getSuccessorBitSet(   code, state);
    default                : __assume(0);
    }
    return 0;
  }

// ----------------------------------- action functions ---------------------------------
  inline const TerminalType *getTermArray(       UINT code) const {
    return m_termArrayTable  + (code & 0x7fff);
  }
  inline const  BYTE        *getTermBitSet(      UINT code) const {
    return m_termBitSetTable + (code & 0x7fff);
  }

// --------------------------- getAction -------------------------------------
  inline        int getActionBinSearch(          UINT code, UINT term) const {
    const int index = findArrayIndex(getTermArray(code), term);
    return (index >= 0) ? m_actionArrayTable[(code >> 17) + index] : _ParserError;
  }
  inline        int getActionSplitNode(          UINT code, UINT term) const {
    const int a = getActionFromCode(leftChild(m_actionCodeArray, code), term);
    return (a != _ParserError) ? a : getActionFromCode(rightChild(m_actionCodeArray, code), term);
  }
  static inline int getActionImmediate(          UINT code, UINT term)       {
    return ((code & 0x7fff) == term)        ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionBitSet(             UINT code, UINT term) const {
    return bitsetContains(getTermBitSet(code), termBitSetCapacity, term) ? ((signed int)code >> 17) : _ParserError;
  }
  inline        int getActionFromCode(           UINT code, UINT term) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getActionBinSearch(code, term);
    case CompCodeSplitNode : return getActionSplitNode(code, term);
    case CompCodeImmediate : return getActionImmediate(code, term);
    case CompCodeBitSet    : return getActionBitSet(   code, term);
    default                : __assume(0);
    }
  }

public:
#pragma warning(push)

  // term is a terminal-symbol.
  // Return action
  //   action >  0 : Shift to state = action
  //   action <  0 : Reduce by production p = -action;
  //   action == 0 : Accept, ie. reduce by production 0
  //   _ParserError: Unexpected term
  int getAction(          UINT state, UINT term     ) const final {
    assert(state < stateCount);
    return getActionFromCode(m_actionCodeArray[state], term);
  }

  // nterm is nonterminal
  UINT getSuccessor(      UINT state, UINT nterm    ) const final {
    assert(state < stateCount);
    assert((nterm >= termCount) && (nterm < symbolCount));
    return getSuccessorFromCode(m_successorCodeArray[nterm-termCount], state);
  }

#pragma warning(pop)

  ParserTablesTemplateTransSucc(
                       const BYTE          *prodLengthArray
                      ,const NTIndexType   *leftSideArray
                      ,const SymbolType    *rightSideTable
                      ,const char          *nameString
                      ,const UINT          *actionCodeArray
                      ,const TerminalType  *termArrayTable
                      ,const ActionType    *actionArrayTable
                      ,const BYTE          *termBitSetTable
                      ,const UINT          *successorCodeArray
                      ,const StateType     *stateArrayTable
                      ,const StateType     *newStateArrayTable
                      ,const BYTE          *stateBitSetTable
                      )
    :GenereratedTablesCommon(prodLengthArray  , leftSideArray
                            ,rightSideTable   , nameString
                            )
    ,m_actionCodeArray    (actionCodeArray    )
    ,m_termArrayTable     (termArrayTable     )
    ,m_actionArrayTable   (actionArrayTable   )
    ,m_termBitSetTable    (termBitSetTable    )
    ,m_successorCodeArray (successorCodeArray )
    ,m_stateArrayTable    (stateArrayTable    )
    ,m_newStateArrayTable (newStateArrayTable )
    ,m_stateBitSetTable   (stateBitSetTable   )
  {
  }
};
