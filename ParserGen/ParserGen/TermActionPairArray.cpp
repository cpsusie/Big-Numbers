#include "stdafx.h"
#include "TermActionPairArray.h"

TermActionPairArray &TermActionPairArray::sortByTerm() {
  sort(termActionPairCompareTerm);
  return *this;
}

TermSet TermActionPairArray::getLegalTermSet(UINT terminalCount) const {
  TermSet result(terminalCount);
  for(const TermActionPair tap : *this) {
    assert(!result.contains(tap.getTerm()));
    result.add(tap.getTerm());
  }
  return result;
}

ActionArray TermActionPairArray::getActionArray() const {
  ActionArray result(size());
  for(const TermActionPair tap : *this) {
    result.add(tap.getAction());
  }
  return result;
}

Action TermActionPairArray::getAction(UINT term) const {
  const TermActionPair key(term, PA_SHIFT, 0);
  const intptr_t       index = binarySearch(key, termActionPairCompareTerm);
  return (index < 0) ? Action() : ((*this)[index]).getAction();
}

TermActionPairArray TermActionPairArray::selectReduceTermActionPairs() const {
  TermActionPairArray result;
  for(TermActionPair tap : *this) {
    if(tap.isReduceAction()) {
      result.add(tap);
    }
  }
  return result;
}

TermActionPairArray TermActionPairArray::selectShiftTermActionPairs() const {
  TermActionPairArray result;
  for(TermActionPair tap : *this) {
    if(tap.isShiftAction()) {
      result.add(tap);
    }
  }
  return result;
}

String TermActionPairArray::toString(const AbstractSymbolNameContainer &nameContainer) const {
  String result;
  for(const TermActionPair tap : *this) {
    result += format(_T("   %s\n"), tap.toString(nameContainer).cstr());
  }
  return result;
}

