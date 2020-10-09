#include "pch.h"
#include <BitSet.h>
#include <Random.h>

size_t BitSet::select(RandomGenerator &rnd) const {
  const size_t i = randSizet(m_capacity, rnd);
  for(ConstIterator<size_t> it = getIterator(i); it.hasNext();) {
    return it.next();
  }
  for(ConstIterator<size_t> it = getReverseIterator(i); it.hasNext();) {
    return it.next();
  }
  throwSelectFromEmptyCollectionException(__TFUNCTION__);
  return -1;
}
