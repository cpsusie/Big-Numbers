#include "stdafx.h"
#include "UIntPermutation.h"

UIntPermutation::UIntPermutation(size_t capacity) {
  init((UINT)capacity);
}

UIntPermutation::UIntPermutation() {
}

void UIntPermutation::init(UINT capacity) {
  setCapacity(capacity).clear(-1).insert(0, (UINT)-1, capacity);
}

UINT UIntPermutation::findIndex(UINT v) const {
  if(v >= size()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("v=%u, size=%zu"), v, size());
  }
  const size_t n = size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i] == v) {
      return i;
    }
  }
  throwException(_T("%s dropped to the end! v=%u"), __TFUNCTION__, v);
  return -1;
}

void UIntPermutation::validate() const {
  if(isEmpty()) {
    return;
  }
  BitSet s(size());
  for(UINT v : *this) {
    if(v >= size()) {
      throwException(_T("%s:A value (=%u) is outside valid range [0..%zu]"), __TFUNCTION__, v, size());
    }
    if(s.contains(v)) {
      throwException(_T("%s:A value (=%u) is duplicated. All values in permutation must be distinct"), __TFUNCTION__, v);
    }
    s.add(v);
  }
  assert(s.size() == size());
}
