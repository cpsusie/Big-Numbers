#pragma once

#include "TermSetReduction.h"
#include "ReduceNode.h"

class ReduceNodeBitSet : public ReduceNode {
private:
  const TermSetReduction m_termSetReduction;
public:
  ReduceNodeBitSet(const ReduceNode *parent, const ReduceNodeBase &base, const TermSetReduction &termSetReduction)
    : ReduceNode(parent, base, termSetReduction.getLegalTermCount(), CompCodeBitSet)
    , m_termSetReduction(termSetReduction)
  {
  }
  const TermSetReduction &getTermSetReduction() const override {
    return m_termSetReduction;
  }
  String toString() const override;
};
