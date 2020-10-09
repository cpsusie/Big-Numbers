#pragma once

#include <HashMap.h>
#include "HashSetIterator.h"
#include "HashMapNode.h"

class HashMapIterator : public HashSetIterator {
public:
  HashMapIterator(const HashMapImpl *map) : HashSetIterator(map) {
  }
  AbstractIterator *clone() override {
    return new HashMapIterator(*this);
  }
  void             *next()  override {
    return (AbstractEntry*)((HashMapNode*)nextNode());
  }
};
