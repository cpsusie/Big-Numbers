#include "stdafx.h"
#include "NTermNewStatePairArray.h"

NTermNewStatePairArray &NTermNewStatePairArray::sortByNTerm() {
  sort(ntermNewStatePairCompareNTerm);
  return *this;
}

int NTermNewStatePairArray::getNewState(UINT nterm) const {
  const NTermNewStatePair key(nterm, 0);
  const intptr_t          index = binarySearch(key, ntermNewStatePairCompareNTerm);
  return (index < 0) ? -1 : (*this)[index].getNewState();
}

String NTermNewStatePairArray::toString(const AbstractSymbolNameContainer &nameContainer) const {
  String result;
  for(const NTermNewStatePair ntns : *this) {
    result += format(_T("   %s\n"), ntns.toString(nameContainer).cstr());
  }
  return result;
}
