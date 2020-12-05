#pragma once

#include "StatePair.h"
#include "StateArray.h"

// Array of StatePairs belonging to 1 LR-state ordered by term. To get the action for a given terminal term, call getAction(term)
// which make a binary search for term in the array
class StatePairArray : public CompactArray<StatePair> {
public:
  inline StatePairArray() {
  }
  inline StatePairArray(UINT capacity) : CompactArray(capacity) {
  }

  StatePairArray      &sortByFromState();

  StatePairArray      &sortByNewState();
  // Return all legal from-states in array as a BitSet
  StateSet             getFromStateSet(UINT stateCount) const;
  // Return all (no neccessary distinct) newstates in array
  StateArray           getNewStateArray()               const;
  // = size() = number of different from-states in array
  inline UINT          getFromStateCount()              const {
    return(UINT)size();
  }
  // Number of different to-states in array
  UINT                 getNewStateCount()               const;
  String               toString()                       const;
};

