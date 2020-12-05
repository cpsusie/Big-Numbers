#include "stdafx.h"
#include "TermSetReductionArray.h"

UINT TermSetReductionArray::getLegalTermCount() const {
  UINT sum = 0;
  for(auto it = getIterator(); it.hasNext();) {
    sum += it.next().getLegalTermCount();
  }
  return sum;
}

static inline int legalTermCountReverseCmp(const TermSetReduction &i1, const TermSetReduction &i2) {
  return (int)i2.getLegalTermCount() - (int)i1.getLegalTermCount();
}

void TermSetReductionArray::sortByLegalTermCount() {
  if(size() > 1) {
    sort(legalTermCountReverseCmp);
  }
}

String TermSetReductionArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += format(_T("   %s\n"), it.next().toString().cstr());
  }
  return result;
}
