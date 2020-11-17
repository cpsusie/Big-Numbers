#include "pch.h"
#include <BitSet.h>

void BitSet::getRangeTable(CompactInt64Array &rangeTable, BYTE shift) const {
  rangeTable.clear();
  const UINT         stepSize     = 1 << shift;
  size_t             currentLimit = stepSize;
  size_t             counter      = 0;
  for(auto it = getIterator(); it.hasNext();) {
    const size_t e = it.next();
    if(++counter >= currentLimit) {

//      printf("hashTable[%3d]:%11s -> %s\n", m_rangeTable.size(), format1000(currentLimit).cstr(), format1000(e).cstr());

      rangeTable.add(e+1);
      currentLimit += stepSize;
    }
  }
}
