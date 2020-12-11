#pragma once

#include <NumberInterval.h>
#include <BitSet.h>

class BitSetInterval : public UIntInterval {
public:
  BitSetInterval() : UIntInterval(0,0) {
  }
  BitSetInterval(const UIntInterval &src) : UIntInterval(src) {
  }
  BitSetInterval(UINT minValue, UINT capacity) : UIntInterval(minValue, minValue+capacity) {
  }
  inline UINT getCapacity() const {
    return getLength();
  }
  inline UINT getSizeofBitSet() const {
    return SearchFunctions::getSizeofBitSet(getCapacity());
  }
  inline bool isEmpty() const {
    return getCapacity() == 0;
  }
  inline void setEmpty() {
    clear();
  }
  // Return true, if set doesn't contain any 1-bits at positions outside range [getFrom()..getTo()-1],
  // else return false
  bool checkBitSetValues(const BitSet &set) const;
  // Return a BitSet, with all 1-bits in range getFrom()..getTo()-1, and capacity = max(1,getTo())
  BitSet createBitSetMask() const;
  String toString() const {
    return isEmpty() ? _T("empty") : format(_T("[%u-%u]"), getFrom(), getTo() - 1);
  }
};
