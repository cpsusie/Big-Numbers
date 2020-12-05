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
  ~ReduceNodeSplit() override;
  ReduceNodeSplit &setChild(BYTE index, ReduceNode *child);
  const ReduceNode &getChild(BYTE index) const override {
    assert(index < 2);
    return *m_child[index];
  }
  String toString() const override;
};
