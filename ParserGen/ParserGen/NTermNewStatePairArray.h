#pragma once

#include "StateArray.h"
#include "NTermNewStatePair.h"

class NTermNewStatePairArray : public CompactArray<NTermNewStatePair> {
public:
  NTermNewStatePairArray() {
  }
  NTermNewStatePairArray(UINT capacity) : CompactArray(capacity) {
  }
  NTermNewStatePairArray &sortByNTerm();
  inline UINT             getLegalNTermCount()                                       const {
    return(UINT)size();
  }
  inline UINT             getNewStateCount()                                         const {
    return(UINT)size();
  }
  // return >= 0 if new state exist for given nterm, else return -1
  int                     getNewState(UINT nterm)                                    const;
  String                  toString(const AbstractSymbolNameContainer &nameContainer) const;
};
