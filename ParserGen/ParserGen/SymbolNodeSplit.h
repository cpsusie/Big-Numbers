#pragma once

#include "SymbolNode.h"

class SymbolNodeSplit : public SymbolNode {
private:
  const SymbolNode *m_child[2];
public:
  SymbolNodeSplit(const SymbolNode *parent, const SymbolNodeBase &base, UINT fromStateCount)
    : SymbolNode(parent, base, fromStateCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~SymbolNodeSplit() final;
  SymbolNodeSplit &setChild(BYTE index, SymbolNode *child);
  const SymbolNode &getChild(BYTE index) const final {
    assert(index < 2);
    return *m_child[index];
  }
  void addAllBitSets(BitSetArray &a) const final;

  String toString() const final;
};
