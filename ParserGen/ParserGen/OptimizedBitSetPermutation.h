#pragma once

#include "ByteCount.h"
#include "BitSetInterval.h"
#include "UIntPermutation.h"

// find permutation of elements in this, so that all 1-bits will be put first in bitset
class OptimizedBitSetPermutation : public UIntPermutation {
private:
  UINT m_newCapacity;
  // Return this
  OptimizedBitSetPermutation &addSet(const BitSet &s, UINT &v);
  OptimizedBitSetPermutation &setNewCapacity(UINT v);
public:
  OptimizedBitSetPermutation() : m_newCapacity(0) {
  }
  OptimizedBitSetPermutation(const BitSet &bitSet);
  inline UINT getNewCapacity() const {
    return m_newCapacity;
  }
  ByteCount getSavedBytesByOptimizedBitSets(UINT bitSetCount) const;
  void validate() const override;
};
