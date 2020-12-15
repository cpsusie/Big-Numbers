#pragma once

#include "ByteCount.h"
#include "BitSetInterval.h"
#include "UIntPermutation.h"

class OptimizedBitSetPermutation2 : public UIntPermutation {
private:
  BitSetInterval m_interval[2];
  BitSetInterval &getInterval(BYTE index);
  // Return this
  OptimizedBitSetPermutation2 &addSet(const BitSet &s, UINT &v             );
  // Return this
  OptimizedBitSetPermutation2 &setStart(   BYTE index, UINT start          );
  // Return this
  OptimizedBitSetPermutation2 &setEnd(     BYTE index, UINT end            );
  // Return this
  OptimizedBitSetPermutation2 &setInterval(BYTE index, UINT start, UINT end);
  void setEmpty();
public:
  OptimizedBitSetPermutation2() {
  }
  // Find permutation of elements.
  // Assume A.capacity == B.capacity
  // First find S0 = A - B, S1 = A & B, S2 = B - A, S3 = not(A | B). notice: S0..3 are all mutually disjoint, and union(S0..3) == {0,1,...capacity-1}
  // If S0 is empty, put elements from S1, then S2,S4
  // If S2 is empty, put elements from S1, then S0,S4
  // If S3 is empty, no permutation is done because no space can be saved...all bits are used somewhere
  // If all are non-empty, put elements S0 first, then elements S1,S2,S3
  OptimizedBitSetPermutation2(const BitSet &A, const BitSet &B);
  inline const BitSetInterval &getInterval(BYTE index) const {
    assert(index < ARRAYSIZE(m_interval));
    return m_interval[index];
  }
  inline UINT getCapacity(BYTE index) const {
    return getInterval(index).getCapacity();
  }
  ByteCount getSavedBytesByOptimizedBitSets(UINT Acount, UINT Bcount) const;
  void validate() const override;
};
