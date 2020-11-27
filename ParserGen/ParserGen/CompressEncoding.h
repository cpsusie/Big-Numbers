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
 ,BC_TERMARRAYTABLE
 ,BC_ACTIONARRAYTABLE
 ,BC_TERMBITSETTABLE
 ,BC_SUCCESSORCODEARRAY
 ,BC_NTINDEXARRAYTABLE
 ,BC_NEWSTATEARRAYTABLE
 ,BC_STATEARRAYTABLE
 ,BC_STATEBITSETTABLE
} ByteCountTableType;

class Grammar;

class TableTypeByteCountMap : public CompactHashMap<CompactKeyType<ByteCountTableType> , ByteCount, 5> {
private:
  UINT m_termBitSetCapacity;
  UINT m_stateBitSetCapacity;
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
  inline UINT getStateBitSetCapacity() const {
    return m_stateBitSetCapacity;
  }
  String getTableString(ByteCountTableType type) const;

};

class Grammar;
class AllTemplateTypes {
private:
  const IntegerType m_symbolType, m_termType, m_NTindexType, m_actionType, m_stateType;
public:
  AllTemplateTypes(const Grammar              &grammar);
  AllTemplateTypes(const AbstractParserTables &tables );

  inline IntegerType getSymbolType()  const { return m_symbolType;  }
  inline IntegerType getTermType()    const { return m_termType;    }
  inline IntegerType getNTindexType() const { return m_NTindexType; }
  inline IntegerType getActionType()  const { return m_actionType;  }
  inline IntegerType getStateType()   const { return m_stateType;   }
};
