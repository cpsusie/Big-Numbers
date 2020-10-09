#include "pch.h"
#include <Random.h>
#include <Array.h>

void ArrayImpl::shuffle(size_t from, size_t count, RandomGenerator &rnd) {
  if(from >= size()) {
    return;
  }
  count = getSortCount(from, count);
  for(size_t i = from; i < count; i++ ) {
    std::swap(m_elem[i], m_elem[from + randSizet(count, rnd)]);
  }
  m_updateCount++;
}
