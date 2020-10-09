#include "pch.h"
#include <HashMap.h>
#include "HashMapIterator.h"

AbstractIterator *HashMapImpl::getIterator() const {
  return new HashMapIterator(this);
}
