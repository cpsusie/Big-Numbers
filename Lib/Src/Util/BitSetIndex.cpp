#include "pch.h"
#include <Math.h>
#include <BitSet.h>

BitSetIndex::BitSetIndex(const BitSet &src) : m_bitSet(src) {
  rebuildRangeTable();
}

void BitSetIndex::rebuildRangeTable() {
  m_rangeTable.clear();
  const size_t capacity = m_bitSet.getCapacity();
  m_shift = 0;
  if(capacity >= 256) {
    for(size_t t = capacity>>8; t && m_shift < 12; m_shift++, t >>= 1);
    m_bitSet.getRangeTable(m_rangeTable, m_shift);
  }

/*
  printf("BitSetIndex:.BitSet.capacity:%11u. size:%-11s, Shift:%3d. RangeTable.size=%11u\n"
        ,capacity
        ,format1000(m_bitSet.size()).cstr()
        ,m_shift
        ,m_rangeTable.size());
*/

}

intptr_t BitSetIndex::getIndex(size_t i) const {
  if(!m_bitSet.contains(i)) {
    return -1;
  }
  const intptr_t rangeIndex = m_rangeTable.binarySearchLE(i, sizetHashCmp);
  if(rangeIndex < 0) {
    return m_bitSet.getCount(0, i) - 1;
  } else {
    return ((rangeIndex+1) << m_shift) + m_bitSet.getCount(m_rangeTable[rangeIndex], i) - 1;
  }
}

String BitSetIndex::getInfoString() const {
  return format(_T("\nBitStIndex:info\nBitSet.capacity:%s (in bytes:%s)\nRangeTable.size:%s (in bytes:%s)\n")
               ,format1000(m_bitSet.getCapacity()).cstr()
               ,format1000(m_bitSet.getCapacity()/_BITSET_ATOMSIZE).cstr()
               ,format1000(m_rangeTable.size()).cstr()
               ,format1000(m_rangeTable.size()*sizeof(UINT)).cstr()
               );
}
