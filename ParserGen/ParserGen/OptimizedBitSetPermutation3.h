#pragma once

#include "ByteCount.h"
#include "UIntPermutation.h"

class OptimizedBitSetPermutation3 : public UIntPermutation {
public:
  OptimizedBitSetPermutation3() {
  }
  OptimizedBitSetPermutation3(const BitSetArray &sets);
  ByteCount getSavedBytesByOptimizedBitSets() const;
};
