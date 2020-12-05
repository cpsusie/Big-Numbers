#include "stdafx.h"

#include "BitSetInterval.h"

BitSet BitSetInterval::createBitSetMask() const {
  const UINT capacity = max(1, getTo());
  BitSet result(capacity);
  if(getTo() > getFrom()) {
    result.add(getFrom(), getTo() - 1);
  }
  return result;
}

bool BitSetInterval::checkBitSetValues(const BitSet &set) const {
  if((getFrom() == 0) && set.getCapacity() <= getCapacity()) {
    return true;
  }
  BitSet tmp(set.getCapacity());
  if(getTo() > getFrom()) {
    tmp.add(getFrom(), getTo() - 1);
  }
  return (set & tmp.invert()).isEmpty();
}
