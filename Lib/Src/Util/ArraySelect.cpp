#include "pch.h"
#include <Random.h>
#include <Array.h>

void *ArrayImpl::select(RandomGenerator &rnd) const {
  if(size() == 0) emptyArrayError(__TFUNCTION__);
  return m_elem[randSizet(m_size, rnd)];
}
