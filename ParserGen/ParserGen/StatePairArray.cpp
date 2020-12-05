#include "stdafx.h"
#include "StatePairArray.h"
#include <CompactHashSet.h>

static int statePairCompareFromState(const StatePair &p1, const StatePair &p2) {
  return (int)p1.m_fromState - (int)p2.m_fromState;
}

static int statePairCmpByNewState(const StatePair &p1, const StatePair &p2) {
  int c = (int)p1.m_newState - (int)p2.m_newState;
  if(c) return c;
  return (int)p1.m_fromState - (int)p2.m_fromState;
}

StatePairArray &StatePairArray::sortByFromState() {
  sort(statePairCompareFromState);
  return *this;
}

StatePairArray &StatePairArray::sortByNewState() {
  sort(statePairCmpByNewState);
  return *this;
}

StateSet StatePairArray::getFromStateSet(UINT stateCount) const {
  StateSet result(stateCount);
  for(const StatePair sp : *this) {
    result.add(sp.m_fromState);
  }
  return result;
}

StateArray StatePairArray::getNewStateArray() const {
  StateArray result(size());
  for(const StatePair sp : *this) {
    result.add(sp.m_newState);
  }
  return result;
}

UINT StatePairArray::getNewStateCount() const {
  CompactUIntHashSet<100> set;
  for(StatePair p : *this) {
    set.add(p.m_newState);
  }
  const UINT countedResult = (UINT)set.size();
  if(countedResult != size()) {
    int fisk = 1;
  }
  return (UINT)size();
}

String StatePairArray::toString() const {
  String result;
  for(const StatePair sp : *this) {
    result += format(_T("   %s\n"), sp.toString().cstr());
  }
  return result;
}
