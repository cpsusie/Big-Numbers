#include "pch.h"
#include "TreeMapNode.h"
#include <TreeMap.h>

AbstractEntry *TreeMapImpl::selectEntry(RandomGenerator &rnd) const {
  if(size() == 0) throwSelectFromEmptyCollectionException(__TFUNCTION__);
  return (TreeMapNode*)findNode(select(rnd));
}
