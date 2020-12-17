#include "stdafx.h"
#include "OptimizedBitSetPermutation3.h"

class IndexCount {
public:
  UINT m_index, m_count;
  inline IndexCount(UINT index=0) : m_index(index), m_count(0) {
  }
};

static int indexCountCmpByCount(const IndexCount &ic1, const IndexCount &ic2) {
  int c = (int)ic2.m_count - (int)ic1.m_count;
  if(c) return c;
  return (int)ic1.m_index - (int)ic2.m_index;
}

OptimizedBitSetPermutation3::OptimizedBitSetPermutation3(const BitSetArray &sets)
  : UIntPermutation(sets[0].getCapacity())
{
  const UINT n = getOldCapacity();
  CompactArray<IndexCount> bitCounters(n);
  for(UINT i = 0; i < n; i++) {
    bitCounters.add(i);
  }
  for(auto it = sets.getIterator(); it.hasNext();) {
    const BitSet &s = it.next();
    if(s.getCapacity() != n) {
      throwInvalidArgumentException(__TFUNCTION__, _T("All sets must have same capacity"));
    }
    for(auto sit = s.getIterator(); sit.hasNext();) {
      const size_t v = sit.next();
      bitCounters[v].m_count++;
    }
  }
  bitCounters.sort(indexCountCmpByCount);
  for(UINT i = 0; i < n; i++) {
    (*this)[i] = bitCounters[i].m_index;
  }
}

ByteCount OptimizedBitSetPermutation3::getSavedBytesByOptimizedBitSets() const {
  return ByteCount();
}
