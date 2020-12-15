#include "stdafx.h"
#include "OptimizedBitSetPermutation.h"

//#define TEST_BITSETPERMUTATION

OptimizedBitSetPermutation::OptimizedBitSetPermutation(const BitSet &bitSet)
: UIntPermutation(bitSet.getCapacity())
{
  BitSet compl(bitSet);
  compl.invert();
  UINT index = 0;

//  debugLog(_T("%s:bitSet:%s\n"       ), __TFUNCTION__, bitSet.toRangeString().cstr());
//  debugLog(_T("%s:compl(bitSet):%s\n"), __TFUNCTION__, compl.toRangeString().cstr());

  addSet(bitSet, index);
  setNewCapacity(index);
  addSet(compl, index);
}

OptimizedBitSetPermutation &OptimizedBitSetPermutation::addSet(const BitSet &s, UINT &v) {
  for(auto it = s.getIterator(); it.hasNext();) {
    (*this)[it.next()] = v++;
  }
  return *this;
}

OptimizedBitSetPermutation &OptimizedBitSetPermutation::setNewCapacity(UINT v) {
  m_newCapacity = v;
  return *this;
}

ByteCount OptimizedBitSetPermutation::getSavedBytesByOptimizedBitSets(UINT bitSetCount) const {
  const UINT oldCapacity = getOldCapacity(), newCapacity = getNewCapacity();
  if(newCapacity >= oldCapacity) {
    return ByteCount();
  } else {
    const ByteCount oldByteCount = ByteCount::wordAlignedSize(bitSetCount * oldCapacity);
    const ByteCount newByteCount = ByteCount::wordAlignedSize(bitSetCount * newCapacity);
    return oldByteCount - newByteCount;
  }
}

void OptimizedBitSetPermutation::validate() const {
  __super::validate();
  if((m_newCapacity == 0) || (m_newCapacity > getOldCapacity())) {
    throwException(_T("%s: newCapacity(=%u) must be in range [1..%u]"), __TFUNCTION__, m_newCapacity, getOldCapacity());
  }
}

