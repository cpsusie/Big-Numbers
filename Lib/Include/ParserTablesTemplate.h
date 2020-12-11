#pragma once

#include "StringArray.h"
#include "AbstractParserTables.h"

namespace LRParsing {

class SearchFunctions {
public:

  typedef BYTE CompressionMethod;

  // 4 different compression-codes. Saved in bit 15-16 in m_actionCode[i] and m_successorCode[i]
  // See generated parsertables for more info of encoding
  static constexpr CompressionMethod CompCodeBinSearch = 0;
  static constexpr CompressionMethod CompCodeSplitNode = 1;
  static constexpr CompressionMethod CompCodeImmediate = 2;
  static constexpr CompressionMethod CompCodeBitSet    = 3;

  // special value for CompCodeImmediate, indicating always return immediate-result, regardless of input
  static constexpr UINT              _NoInputCheck = 0x7fff;

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

  // Return number of bytes neccessary to have a bitSet ranging from [0..capacity-1]
  // if capacity=0, return 0
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
};

template<UINT     codeArraySize
        ,UINT     bitSetIntervalFrom
        ,UINT     bitSetIntervalCapacity
        ,bool     alwaysCheckInput
        ,typename InputType
        ,typename OutputType>
class CompressedMatrix {
private:
  const UINT           *m_codeArray;
  const InputType      *m_inputArrayTable;
  const OutputType     *m_outputArrayTable;
  const BYTE           *m_bitSetTable;

  inline const  InputType *getInputArray(           UINT code) const {
    return m_inputArrayTable + (code & 0x7fff);
  }
  inline const  BYTE      *getBitSet(               UINT code) const {
    return m_bitSetTable     + (code & 0x7fff);
  }
  inline        int        getBinSearchResult(      UINT code, InputType input) const {
    const int index = SearchFunctions::findArrayIndex(getInputArray(code), input);
    return (index >= 0) ? m_outputArrayTable[(code >> 17) + index] : -1;
  }
  static inline int        getImmediateResult(      UINT code, InputType input) {
    const UINT v = code & 0x7fff;
    if(alwaysCheckInput) {
      return (input == v) ? (code >> 17) : -1;
    } else {
      return ((v == SearchFunctions::_NoInputCheck) || (input == v)) ? (code >> 17) : -1;
    }
  }
  inline        int        getBitSetResult(         UINT code, InputType input) const {
    return SearchFunctions::bitsetContains(getBitSet(code), bitSetIntervalCapacity, input-bitSetIntervalFrom) ? (code >> 17) : -1;
  }
  int getResultFromCode(                            UINT code, InputType input) const {
    for(;;) {
      switch(SearchFunctions::getCompressionCode(code)) {
      case SearchFunctions::CompCodeBinSearch : return getBinSearchResult(code, input);
      case SearchFunctions::CompCodeSplitNode :
        { const int result = getResultFromCode(SearchFunctions::leftChild(m_codeArray, code), input);
          if(result >= 0) return result;
          code = SearchFunctions::rightChild(m_codeArray, code);
        }
        break;
      case SearchFunctions::CompCodeImmediate : return getImmediateResult(code, input);
      case SearchFunctions::CompCodeBitSet    : return getBitSetResult(   code, input);
      default                                 : __assume(0);
      }
    }
    return -1;
  }
public:
  CompressedMatrix(const UINT       *codeArray
                  ,const InputType  *inputArrayTable
                  ,const OutputType *outputArrayTable
                  ,const BYTE       *bitSetTable
  )
  :m_codeArray(       codeArray       )
  ,m_inputArrayTable( inputArrayTable )
  ,m_outputArrayTable(outputArrayTable)
  ,m_bitSetTable(     bitSetTable     )
  {
  }
  int getResult(UINT index, InputType input) const {
    return getResultFromCode(m_codeArray[index], input);
  }
};

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
        ,UINT     newStateIntervalFrom  , UINT     newStateIntervalCapacity
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
  CompressedMatrix<termCount , shiftStateIntervalFrom, shiftStateIntervalCapacity, true , StateType   , StateType     > m_shiftMatrix;
  CompressedMatrix<stateCount, 0                     , termBitSetCapacity        , true , TerminalType, ProductionType> m_reduceMatrix;
  CompressedMatrix<termCount , newStateIntervalFrom  , newStateIntervalCapacity  , false, StateType   , StateType     > m_newStateMatrix;
public:
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
                      ,const UINT           *newStateCodeArray
                      ,const StateType      *stateArrayTable
                      ,const StateType      *newStateArrayTable
                      ,const BYTE           *newStateBitSetTable
                      )
    :GenereratedTablesCommon(prodLengthArray, leftSideArray           , rightSideTable        , nameString           )
    ,m_shiftMatrix(   shiftCodeArray        , shiftFromStateArrayTable, shiftToStateArrayTable, shiftStateBitSetTable)
    ,m_reduceMatrix(  reduceCodeArray       , termArrayTable          , reduceArrayTable      , termBitSetTable      )
    ,m_newStateMatrix(newStateCodeArray     , stateArrayTable         , newStateArrayTable    , newStateBitSetTable  )
  {
  }

#pragma warning(push)
  UINT   getStartState() const final {
    return startState;
  }
  // term is a terminal-symbol.
  // Return action
  //   action.getType() = PA_SHIFT : Shift to state action.getNewState()
  //   action.getType() = PA_REDUCE: Reduce by production action.getReduceProduction()
  //   action.getType() = PA_ERROR : Unexpected term...Invalid input in the given state
  //   if action.getType() = PA_REDUCE and reduceProduction() = 0, then Accept input, unless errors has been detected at some earlier stage
  Action getAction(UINT state, UINT term ) const final {
    assert(state < stateCount);
    assert(isTerminal(term));
    const int nextState = m_shiftMatrix.getResult(term, state);
    if(nextState  >= 0) return Action(PA_SHIFT, nextState);
    const int reduceProd = m_reduceMatrix.getResult(state, term);
    if(reduceProd >= 0) return Action(PA_REDUCE, reduceProd);
    return Action();
  }

  // nterm is nonterminal-symbol
  // Return newState (>= 0) if valid combination of state, nterm, or -1 if not
  int    getSuccessor(UINT state, UINT nterm) const final {
    assert( state < stateCount);
    assert(isNonTerminal(nterm));
    return m_newStateMatrix.getResult(NTermToNTIndex(nterm), state);
  }
#pragma warning(pop)

};

}; // namespace LRParsing
