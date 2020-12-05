#include "stdafx.h"
#include "StatePairBitSetArray.h"

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
