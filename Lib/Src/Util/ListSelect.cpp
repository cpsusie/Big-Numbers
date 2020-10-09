#include "pch.h"
#include <LinkedList.h>
#include <Random.h>

void *ListImpl::select(RandomGenerator &rnd) const {
  if(size() == 0) {
    getError(__TFUNCTION__);
  }
  return findNode(randSizet(size(), rnd))->m_data;
}
