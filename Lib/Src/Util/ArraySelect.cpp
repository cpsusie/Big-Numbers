#include "pch.h"
#include <Random.h>
#include <Array.h>

void *ArrayImpl::select(RandomGenerator &rnd) const {
  if(m_size == 0) {
    selectError(__TFUNCTION__);
  }
  return m_elem[randSizet(m_size, rnd)];
}
