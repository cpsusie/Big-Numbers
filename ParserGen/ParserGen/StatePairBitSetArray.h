#pragma once

#include "StatePairBitSet.h"

class StatePairBitSetArray : public Array<StatePairBitSet> {
public:
  StatePairBitSetArray() {
  }
  StatePairBitSetArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  void        sortBySetSize();
  // Return sum(((*this)[i].getFromStateCount()...i=0..size-1)
  UINT        getFromStateCount()  const;
  // = size() = number of different to-states in array
  inline UINT getNewStateCount()   const {
    return (UINT)size();
  }
  String      toString() const;
};
