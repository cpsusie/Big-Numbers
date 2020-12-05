#pragma once

#include "BitSetInterval.h"
#include "ByteCount.h"

typedef enum {
  BC_SHIFTCODEARRAY
 ,BC_REDUCECODEARRAY
 ,BC_SUCCESSORCODEARRAY
 ,BC_TERMARRAYTABLE
 ,BC_REDUCEARRAYTABLE
 ,BC_TERMBITSETTABLE
 ,BC_NEWSTATEARRAYTABLE
 ,BC_STATEARRAYTABLE
 ,BC_STATEBITSETTABLE
} ByteCountTableType;

class Grammar;

class TableTypeByteCountMap : private CompactHashMap<CompactKeyType<ByteCountTableType>, ByteCount, 5> {
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
