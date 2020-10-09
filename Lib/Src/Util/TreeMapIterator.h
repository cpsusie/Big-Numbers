#pragma once

#include "TreeSetIterator.h"
#include "TreeMapNode.h"

class TreeMapIterator : public TreeSetIterator {
public:
  TreeMapIterator(const TreeSetImpl *set) : TreeSetIterator(set) {
  }
  AbstractIterator *clone() override {
    return new TreeMapIterator(*this);
  }
  void             *next()  override {
    return (AbstractEntry*)((TreeMapNode*)nextNode());
  }
};
