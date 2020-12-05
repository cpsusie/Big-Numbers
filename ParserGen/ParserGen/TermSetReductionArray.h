#pragma once

#include "TermSetReduction.h"

class TermSetReductionArray : public Array<TermSetReduction> {
public:
  TermSetReductionArray() {
  }
  TermSetReductionArray(size_t capacity) : Array(capacity) {
  }
  // sort by setSize, decreasing, ie. largest set first
  void        sortByLegalTermCount();
  // Return sum(((*this)[i].getLegalTermCount()...i=0..size-1)
  UINT        getLegalTermCount()  const;
  // = size() = number of different reduceproductions  in array
  inline UINT getProductionCount() const {
    return (UINT)size();
  }
  String      toString() const;
};
