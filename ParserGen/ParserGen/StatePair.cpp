#include "stdafx.h"
#include "StatePair.h"

StatePair::StatePair(UINT fromState, UINT newState, bool shiftText) : m_fromState(fromState), m_newState(newState), m_shiftText(shiftText) {
  if(m_shiftText) {
    assert(m_fromState != NoFromStateCheck);
  }
  assert(m_newState != NoFromStateCheck);
}

String StatePair::toString() const {
  if(m_shiftText) {
    return format(_T("Shift to %u from %u"), m_newState, m_fromState);
  } else {
    return isNoFromStateCheck()
          ? format(_T("Goto %u"        ), m_newState)
          : format(_T("Goto %u from %u"), m_newState, m_fromState);
  }
}

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

String StatePairArray::toString() const {
  String result;
  for(const StatePair sp : *this) {
    result += format(_T("   %s\n"), sp.toString().cstr());
  }
  return result;
}

StatePairBitSet::operator StatePairArray() const {
  StatePairArray result(getFromStateCount());
  for(auto it = m_fromStateSet.getIterator(); it.hasNext();) {
    result.add(StatePair((USHORT)it.next(), m_newState, m_shiftText));
  }
  return result;
}

String StatePairBitSet::toString() const {
  return format(_T("Goto %u from %s (%u states)"), m_newState, m_fromStateSet.toRangeString().cstr(), getFromStateCount());
}


static int setSizeReverseCmp(const StatePairBitSet &sp1, const StatePairBitSet &sp2) {
  return (int)sp2.getFromStateCount() - (int)sp1.getFromStateCount();
}

void StatePairBitSetArray::sortBySetSize() {
  if(size() > 1) {
    sort(setSizeReverseCmp);
  }
}

// Return sum(((*this)[i].getFromStateCount()...i=0..size-1)
UINT StatePairBitSetArray::getFromStateCount() const {
  UINT sum = 0;
  for(auto it = getIterator(); it.hasNext();) {
    sum += it.next().getFromStateCount();
  }
  return sum;
}

String StatePairBitSetArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += format(_T("   %s\n"), it.next().toString().cstr());
  }
  return result;
}
