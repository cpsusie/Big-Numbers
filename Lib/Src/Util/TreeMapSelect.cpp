#include "pch.h"
#include "TreeMapNode.h"
#include <TreeMap.h>

AbstractEntry *TreeMapImpl::selectEntry(RandomGenerator &rnd) const {
  if(size() == 0) throwEmptyMapException(__TFUNCTION__);
  return (TreeMapNode*)findNode(select(rnd));
}
