#pragma once

#include <MapBase.h>
#include "HashSetNode.h"

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
