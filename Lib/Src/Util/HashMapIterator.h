#pragma once

#include <HashMap.h>
#include "HashSetIterator.h"

class HashMapNode : public HashSetNode, public AbstractEntry {
  friend class HashMapImpl;
private:
  void *m_value;
public:
  const void *key()   const override {
    return HashSetNode::key();
  }
  void       *value() const override {
    return m_value;
  }
};

class HashMapIterator : public HashSetIterator {
public:
  HashMapIterator(HashMapImpl &map) : HashSetIterator(map) {
  }
  AbstractIterator *clone() override {
    return new HashMapIterator(*this);
  }
  void             *next()  override {
    return (AbstractEntry*)((HashMapNode*)nextNode());
  }
};
