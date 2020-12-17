#pragma once

#include "ReduceNode.h"

class ReduceNodeSplit : public ReduceNode {
private:
  const ReduceNode *m_child[2];
public:
  ReduceNodeSplit(const ReduceNode *parent, const ReduceNodeBase &base, UINT legalTermCount)
    : ReduceNode(parent, base, legalTermCount, CompCodeSplitNode)
  {
    m_child[0] = m_child[1] = nullptr;
  }
  ~ReduceNodeSplit() final;
  ReduceNodeSplit &setChild(BYTE index, ReduceNode *child);
  const ReduceNode &getChild(BYTE index) const final {
    assert(index < 2);
    return *m_child[index];
  }
  void addAllBitSets(BitSetArray &a) const final;
  String toString() const final;
};
