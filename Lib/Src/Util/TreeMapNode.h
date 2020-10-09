#pragma once

#include <MapBase.h>
#include "TreeSetNode.h"

class TreeMapNode : public TreeSetNode, public AbstractEntry {
  friend class TreeMapImpl;
private:
  void *m_value;
public:
  const void *key()   const override {
    return TreeSetNode::key();
  }
  void       *value() const override {
    return m_value;
  }
};
