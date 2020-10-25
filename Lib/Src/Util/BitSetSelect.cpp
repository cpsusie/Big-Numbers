#include "pch.h"
#include <BitSet.h>
#include <Random.h>

size_t BitSet::select(RandomGenerator &rnd) const {
  const size_t i = randSizet(m_capacity, rnd);
  for(auto it = getIterator(i); it.hasNext();) {
    return it.next();
  }
  for(auto it = getReverseIterator(i); it.hasNext();) {
    return it.next();
  }
  throwEmptySetException(__TFUNCTION__);
  return -1;
}
