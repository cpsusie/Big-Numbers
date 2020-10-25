#include "pch.h"
#include <HashMap.h>
#include "HashMapNode.h"

AbstractEntry *HashMapImpl::selectEntry(RandomGenerator &rnd) const {
  if(size() == 0) throwEmptyMapException(__TFUNCTION__);
  return (HashMapNode*)findNode(select(rnd));
}
