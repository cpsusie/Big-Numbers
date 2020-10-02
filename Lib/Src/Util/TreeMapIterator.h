#pragma once

#include <MapBase.h>
#include "TreeSetIterator.h"

class TreeMapNode : public TreeSetNode, public AbstractEntry {
  friend class TreeMapImpl;
private:
  void *m_value;
public:
  const void *key()   const override {
    return TreeSetNode::key();
  }
        void *value()       override {
    return m_value;
  }
  const void *value() const override {
    return m_value;
  }
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
