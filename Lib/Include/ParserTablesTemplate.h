#pragma once

#include "StringArray.h"
#include "AbstractParserTables.h"

namespace LRParsing {

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

// -------------------------------------------------------------------------------------------------------------------

template<UINT     symbolCount
        ,UINT     termCount
        ,UINT     productionCount
        ,UINT     stateCount
        ,UINT     tableByteCountx86
        ,UINT     tableByteCountx64
        ,UINT     startState
        ,UINT     termBitSetCapacity
        ,UINT     shiftStateIntervalFrom, UINT     shiftStateIntervalCapacity
        ,UINT     succStateIntervalFrom , UINT     succStateIntervalCapacity
        ,typename SymbolType       /* unsigned, values:[0..symbolCount [ */
        ,typename TerminalType     /* unsigned, values:[0..termCount   [ */
        ,typename NTIndexType      /* unsigned, values:[0..ntermCount  [ */
        ,typename ProductionType   /* unsigned, values:[0..prodCount   [ */
        ,typename StateType>       /* unsigned, values:[0..stateCount  [ */
class ParserTablesTemplateTransShift
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
  const UINT           *m_shiftCodeArray;
  const StateType      *m_shiftFromStateArrayTable;
  const StateType      *m_shiftToStateArrayTable;
  const BYTE           *m_shiftStateBitSetTable;
  const UINT           *m_reduceCodeArray;
  const TerminalType   *m_termArrayTable;
  const ProductionType *m_reduceArrayTable;
  const BYTE           *m_termBitSetTable;
  const UINT           *m_successorCodeArray;
  const StateType      *m_stateArrayTable;
  const StateType      *m_newStateArrayTable;
  const BYTE           *m_stateBitSetTable;

// ----------------------------- successor functions ---------------------------------------
  inline const  StateType   *getStateArray(      UINT code) const {
    return m_stateArrayTable + (code & 0x7fff);
  }
  inline const  BYTE        *getStateBitSet(     UINT code) const {
    return m_stateBitSetTable  + (code & 0x7fff);
  }
  inline        int getSuccessorBinSearch(      UINT code, UINT state) const {
    const int index = findArrayIndex(getStateArray(code), state);
    return (index >= 0) ? m_newStateArrayTable[(code >> 17) + index] : -1;
  }
  inline        int getSuccessorSplitNode(      UINT code, UINT state) const {
    const int s = getSuccessorFromCode(leftChild(m_successorCodeArray, code), state);
    return (s >= 0) ? s : getSuccessorFromCode(rightChild(m_successorCodeArray, code), state);
  }
  static inline int getSuccessorImmediate(      UINT code, UINT state) {
    const UINT fromState = code & 0x7fff;
    return ((fromState == _NoFromStateCheck) || (state == fromState)) ? (code >> 17) : -1;
  }
  inline        int getSuccessorBitSet(         UINT code, UINT state) const {
    return bitsetContains(getStateBitSet(code), succStateIntervalCapacity, state-succStateIntervalFrom) ? (code >> 17) : -1;
  }
  inline        int getSuccessorFromCode(       UINT code, UINT state) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getSuccessorBinSearch(code, state);
    case CompCodeSplitNode : return getSuccessorSplitNode(code, state);
    case CompCodeImmediate : return getSuccessorImmediate(code, state);
    case CompCodeBitSet    : return getSuccessorBitSet(   code, state);
    default                : __assume(0);
    }
    return -1;
  }

// ----------------------------- shift functions ---------------------------------------
  inline const  StateType   *getShiftFromStateArray(  UINT code) const {
    return m_shiftFromStateArrayTable + (code & 0x7fff);
  }
  inline const  BYTE        *getShiftStateBitSet(     UINT code) const {
    return m_shiftStateBitSetTable  + (code & 0x7fff);
  }
  inline        int getShiftBinSearch(      UINT code, UINT state) const {
    const int index = findArrayIndex(getShiftFromStateArray(code), state);
    return (index >= 0) ? m_shiftToStateArrayTable[(code >> 17) + index] : -1;
  }
  inline        int getShiftSplitNode(      UINT code, UINT state) const {
    const int a = getShiftFromCode(leftChild(m_shiftCodeArray, code), state);
    return (a >= 0) ? a : getShiftFromCode(rightChild(m_shiftCodeArray, code), state);
  }
  static inline int getShiftImmediate(      UINT code, UINT state) {
    const UINT fromState = code & 0x7fff;
    return ((fromState == _NoFromStateCheck) || (state == fromState)) ? (code >> 17) : -1;
  }
  inline        int getShiftBitSet(         UINT code, UINT state) const {
    return bitsetContains(getShiftStateBitSet(code), shiftStateIntervalCapacity, state-shiftStateIntervalFrom) ? (code >> 17) : -1;
  }
  inline        int getShiftFromCode(       UINT code, UINT state) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getShiftBinSearch(code, state);
    case CompCodeSplitNode : return getShiftSplitNode(code, state);
    case CompCodeImmediate : return getShiftImmediate(code, state);
    case CompCodeBitSet    : return getShiftBitSet(   code, state);
    default                : __assume(0);
    }
    return -1;
  }

  // ----------------------------------- reduce functions ---------------------------------
  inline const TerminalType *getTermArray(       UINT code) const {
    return m_termArrayTable  + (code & 0x7fff);
  }
  inline const  BYTE        *getTermBitSet(      UINT code) const {
    return m_termBitSetTable + (code & 0x7fff);
  }

// --------------------------- getReduce -------------------------------------
  inline        int getReduceBinSearch(          UINT code, UINT term) const {
    const int index = findArrayIndex(getTermArray(code), term);
    return (index >= 0) ? m_reduceArrayTable[(code >> 17) + index] : -1;
  }
  inline        int getReduceSplitNode(          UINT code, UINT term) const {
    const int p = getReduceFromCode(leftChild(m_reduceCodeArray, code), term);
    return (p >= 0) ? p : getReduceFromCode(rightChild(m_reduceCodeArray, code), term);
  }
  static inline int getReduceImmediate(          UINT code, UINT term)       {
    return ((code & 0x7fff) == term)        ? (code >> 17) : -1;
  }
  inline        int getReduceBitSet(             UINT code, UINT term) const {
    return bitsetContains(getTermBitSet(code), termBitSetCapacity, term) ? (code >> 17) : -1;
  }
  inline        int getReduceFromCode(           UINT code, UINT term) const {
    switch(getCompressionCode(code)) {
    case CompCodeBinSearch : return getReduceBinSearch(code, term);
    case CompCodeSplitNode : return getReduceSplitNode(code, term);
    case CompCodeImmediate : return getReduceImmediate(code, term);
    case CompCodeBitSet    : return getReduceBitSet(   code, term);
    default                : __assume(0);
    }
  }

public:
#pragma warning(push)
  UINT          getStartState() const {
    return startState;
  }
  // term is a terminal-symbol.
  // Return action
  //   action.getType() = PA_SHIFT : Shift to state action.getNewState()
  //   action.getType() = PA_REDUCE: Reduce by production action.getReduceProduction()
  //   action.getType() = PA_ERROR : Unexpected term...Invalid input in the given state
  //   if action.getType() = PA_REDUCE and reduceProduction() = 0, then Accept input, unless errors has been detected at some earlier stage
  Action getAction(      UINT state, UINT term     ) const final {
    assert(state < stateCount);
    assert(isTerminal(term));
    const int nextState = getShiftFromCode(m_shiftCodeArray[term], state);
    if(nextState  >= 0) return Action(PA_SHIFT, nextState);
    const int reduceProd = getReduceFromCode(m_reduceCodeArray[state], term);
    if(reduceProd >= 0) return Action(PA_REDUCE, reduceProd);
    return Action();
  }

  // nterm is nonterminal
  int getSuccessor(      UINT state, UINT nterm    ) const final {
    assert( state < stateCount);
    assert(isNonTerminal(nterm));
    return getSuccessorFromCode(m_successorCodeArray[NTermToNTIndex(nterm)], state);
  }

#pragma warning(pop)

  ParserTablesTemplateTransShift(
                       const BYTE           *prodLengthArray
                      ,const NTIndexType    *leftSideArray
                      ,const SymbolType     *rightSideTable
                      ,const char           *nameString
                      ,const UINT           *shiftCodeArray
                      ,const StateType      *shiftFromStateArrayTable
                      ,const StateType      *shiftToStateArrayTable
                      ,const BYTE           *shiftStateBitSetTable
                      ,const UINT           *reduceCodeArray
                      ,const TerminalType   *termArrayTable
                      ,const ProductionType *reduceArrayTable
                      ,const BYTE           *termBitSetTable
                      ,const UINT           *successorCodeArray
                      ,const StateType      *stateArrayTable
                      ,const StateType      *newStateArrayTable
                      ,const BYTE           *stateBitSetTable
                      )
    :GenereratedTablesCommon(prodLengthArray  , leftSideArray
                            ,rightSideTable   , nameString
                            )
    ,m_shiftCodeArray          (shiftCodeArray           )
    ,m_shiftFromStateArrayTable(shiftFromStateArrayTable )
    ,m_shiftToStateArrayTable  (shiftToStateArrayTable   )
    ,m_shiftStateBitSetTable   (shiftStateBitSetTable    )
    ,m_reduceCodeArray         (reduceCodeArray          )
    ,m_termArrayTable          (termArrayTable           )
    ,m_reduceArrayTable        (reduceArrayTable         )
    ,m_termBitSetTable         (termBitSetTable          )
    ,m_successorCodeArray      (successorCodeArray       )
    ,m_stateArrayTable         (stateArrayTable          )
    ,m_newStateArrayTable      (newStateArrayTable       )
    ,m_stateBitSetTable        (stateBitSetTable         )
  {
  }
};

}; // namespace LRParsing
