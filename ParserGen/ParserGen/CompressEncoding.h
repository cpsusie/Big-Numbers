#pragma once

typedef enum {
  CompCodeBinSearch = AbstractParserTables::CompCodeBinSearch
 ,CompCodeSplitNode = AbstractParserTables::CompCodeSplitNode
 ,CompCodeImmediate = AbstractParserTables::CompCodeImmediate
 ,CompCodeBitSet    = AbstractParserTables::CompCodeBitSet
} CompressionMethod;

const TCHAR *compressMethodToString(CompressionMethod method);

inline UINT encodeCompressMethod(CompressionMethod method) {
  return (((UINT)method) << 15);
}

UINT   encodeValue(     CompressionMethod method, int highEnd, int lowEnd);
String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd);

typedef enum {
  BC_ACTIONCODEARRAY
 ,BC_SHIFTCODEARRAY
 ,BC_REDUCECODEARRAY
 ,BC_SUCCESSORCODEARRAY
 ,BC_TERMARRAYTABLE
 ,BC_ACTIONARRAYTABLE
 ,BC_REDUCEARRAYTABLE
 ,BC_TERMBITSETTABLE
 ,BC_NTINDEXARRAYTABLE
 ,BC_NEWSTATEARRAYTABLE
 ,BC_STATEARRAYTABLE
 ,BC_STATEBITSETTABLE
} ByteCountTableType;

class Grammar;

class TableTypeByteCountMap : private CompactHashMap<CompactKeyType<ByteCountTableType> , ByteCount, 5> {
private:
  UINT           m_termBitSetCapacity;
  BitSetInterval m_shiftStateInterval;
  BitSetInterval m_succStateInterval;
public:
  UINT m_splitNodeCount;
  TableTypeByteCountMap(const Grammar &g);
  ByteCount getSum() const;
  inline UINT getSplitNodeCount() const {
    return m_splitNodeCount;
  }
  inline UINT getTermBitSetCapacity() const {
    return m_termBitSetCapacity;
  }
  inline const BitSetInterval &getShiftStateBitSetInterval() const {
    return m_shiftStateInterval;
  }
  inline const BitSetInterval &getSuccStateBitSetInterval() const {
    return m_succStateInterval;
  }
  void clear() {
    __super::clear();
  }
  String getTableString(ByteCountTableType type) const;
  TableTypeByteCountMap &add(ByteCountTableType type, const ByteCount &bc);
  TableTypeByteCountMap &operator+=(const TableTypeByteCountMap &rhs);
};

class Grammar;
class AllTemplateTypes {
private:
  const IntegerType m_symbolType, m_termType, m_ntIndexType, m_actionType, m_stateType;
public:
  AllTemplateTypes(const Grammar              &grammar);
  AllTemplateTypes(const AbstractParserTables &tables );

  inline IntegerType getSymbolType()  const { return m_symbolType;  }
  inline IntegerType getTermType()    const { return m_termType;    }
  inline IntegerType getNTIndexType() const { return m_ntIndexType; }
  inline IntegerType getActionType()  const { return m_actionType;  }
  inline IntegerType getStateType()   const { return m_stateType;   }
};
