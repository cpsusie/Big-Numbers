#include "stdafx.h"
#include "Grammar.h"
#include "TableTypeByteCountMap.h"

TableTypeByteCountMap::TableTypeByteCountMap(const Grammar &g) {
  m_termBitSetCapacity  = g.getTermBitSetCapacity();
  m_shiftStateInterval  = g.getShiftStateBitSetInterval();
  m_succStateInterval   = g.getSuccStateBitSetInterval();
  m_splitNodeCount      = 0;
}

String TableTypeByteCountMap::getTableString(ByteCountTableType type) const {
  const ByteCount *bc = get(type);
  if(bc) {
    return bc->toStringTableForm().cstr();
  } else {
    return spaceString(ByteCount::tableformWidth);
  }
}

TableTypeByteCountMap &TableTypeByteCountMap::add(ByteCountTableType type, const ByteCount &bc) {
  ByteCount *e = get(type);
  if(e) {
    *e += bc;
  } else {
    put(type, bc);
  }
  return *this;
}

TableTypeByteCountMap &TableTypeByteCountMap::operator+=(const TableTypeByteCountMap &rhs) {
  for(auto it = rhs.getIterator(); it.hasNext();) {
    auto &e = it.next();
    add(e.getKey(), e.getValue());
  }
  return *this;
}

ByteCount TableTypeByteCountMap::getSum() const {
  ByteCount sum;
  for(auto it = getIterator(); it.hasNext();) {
    sum += it.next().getValue();
  }
  return sum;
}

