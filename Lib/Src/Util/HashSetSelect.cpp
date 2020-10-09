#include "pch.h"
#include <HashSet.h>
#include "HashSetTable.h"

void *HashSetImpl::select(RandomGenerator &rnd) const {
  return (void*)m_table->select(rnd)->key();
}
