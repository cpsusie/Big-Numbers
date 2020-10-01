#pragma once

#include <MapBase.h>
#include "TreeSetIterator.h"

class TreeMapNode : public TreeSetNode, public AbstractEntry {
private:
  void *m_value;
public:
  const void *key() const {
    return TreeSetNode::key();
  }
  void *value() {
    return m_value;
  }
  const void *value() const {
    return m_value;
  }
  friend class TreeMapImpl;
};

class TreeMapIterator : public TreeSetIterator {
public:
  TreeMapIterator(TreeSetImpl &set) : TreeSetIterator(set) {
  }
  AbstractIterator *clone() override {
    return new TreeMapIterator(*this);
  }
  void *next()              override {
    return (AbstractEntry*)((TreeMapNode*)nextNode());
  }
};
