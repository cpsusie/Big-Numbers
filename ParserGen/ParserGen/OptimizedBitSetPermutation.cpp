#include "stdafx.h"
#include "IndexMap.h"

OptimizedBitSetPermutation::OptimizedBitSetPermutation(const BitSet &bitSet)
: CompactUIntArray(bitSet.getCapacity())
{
  const UINT oldCapacity = (UINT)bitSet.getCapacity();
  setCapacity(oldCapacity);
  insert(0, (UINT)0, oldCapacity);
  UINT i = 0, index = 0;
  for(auto it = bitSet.getIterator(); it.hasNext();) {
    (*this)[it.next()] = index++;
  }
  BitSet compl(bitSet);
  compl.invert();
  m_newCapacity = index;
  for(auto it = compl.getIterator(); it.hasNext();) {
    (*this)[it.next()] = index++;
  }
}

ByteCount OptimizedBitSetPermutation::getSavedBytesByOptimizedBitSets(UINT bitSetCount) const {
  const UINT oldCapacity = getOldCapacity(), newCapacity = getNewCapacity();
  if(newCapacity < oldCapacity) {
    const ByteCount oldByteCount = ByteCount::wordAlignedSize(bitSetCount * oldCapacity);
    const ByteCount newByteCount = ByteCount::wordAlignedSize(bitSetCount * newCapacity);
    return oldByteCount - newByteCount;
  } else {
    return ByteCount();
  }
}
