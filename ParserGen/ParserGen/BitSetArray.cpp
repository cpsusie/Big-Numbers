#include "stdafx.h"

BitSetArray &BitSetArray::removeDuplicates() {
  const UINT n = (UINT)size();
  if(n < 2) {
    return *this;
  }
  sort(bitSetCmp);
  BitSetArray tmp;
  tmp.setCapacity(n);
  const BitSet *lastSet = &(*this)[0];
  tmp.add(*lastSet);
  for(UINT i = 1; i < n; i++) {
    const BitSet &set = (*this)[i];
    if(set != *lastSet) {
      tmp.add(set);
      lastSet = &set;
    }
  }
  const UINT newSize = (UINT)tmp.size();
  if(newSize != n) {
    *this = tmp;
  }
  return *this;
}
