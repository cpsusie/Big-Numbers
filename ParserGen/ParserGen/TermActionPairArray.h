#pragma once

#include "TermActionPair.h"
#include "ActionArray.h"

// Array of TermActionPairs belonging to 1 LR-state ordered by term. To get the action for a given terminal term, call getAction(term)
// which make a binary search for term in the array
class TermActionPairArray : public CompactArray<TermActionPair> {
public:
  inline TermActionPairArray() {
  }
  inline TermActionPairArray(UINT capacity) : CompactArray(capacity) {
  }

  TermActionPairArray &sortByTerm();
  // Return all legal  terminals in array as a BitSet
  TermSet              getLegalTermSet(UINT terminalCount) const;
  // Return all actions (no neccessary distinct) actions in array
  ActionArray          getActionArray()                    const;
  // = size() = number of different terminal symbols in array
  inline UINT          getLegalTermCount()                 const {
    return(UINT)size();
  }

  // Return action a, a.getType() = PA_SHIFT : Shift to state = a.getNewState()
  //                  a.getType() = PA_REDUCE: Reduce by production = a.getReduceProduction()
  //                  a.getType() = PA_ERROR : Invalid terminal in this state
  // Similar to what AbstractParsertable.getAction(state, term), where this is one row in ActionMatrix[state,term]
  Action               getAction(UINT term)                  const;
  TermActionPairArray  selectReduceTermActionPairs()         const;
  TermActionPairArray  selectShiftTermActionPairs()          const;
  String               toString(const AbstractSymbolNameContainer &nameContainer) const;
};
