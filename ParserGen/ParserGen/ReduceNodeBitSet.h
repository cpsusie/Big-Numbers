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
  const TermSetReduction &getTermSetReduction() const final {
    return m_termSetReduction;
  }
  void addAllBitSets(BitSetArray &a) const final {
    a.add(m_termSetReduction.getTermSet());
  }
  String toString() const final;
};
