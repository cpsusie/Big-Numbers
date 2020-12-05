#pragma once

#include "TermActionPairArray.h"
#include "ReduceNode.h"

class ReduceNodeBinSearch : public ReduceNode {
private:
  const TermActionPairArray m_termActionArray;
public:
  ReduceNodeBinSearch(const ReduceNode *parent, const ReduceNodeBase &base, const TermActionPairArray &termActionArray)
    : ReduceNode(parent, base, termActionArray.getLegalTermCount(), CompCodeBinSearch)
    , m_termActionArray(termActionArray)
  {
  }
  const TermActionPairArray &getTermActionPairArray() const override {
    return m_termActionArray;
  }
  String toString() const override;
};
