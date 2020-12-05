#pragma once

#include "TermActionPair.h"
#include "ReduceNode.h"

class ReduceNodeImmediate : public ReduceNode {
private:
  const TermActionPair m_tap;
public:
  ReduceNodeImmediate(const ReduceNode *parent, const ReduceNodeBase &base, TermActionPair tap)
    : ReduceNode(parent, base, 1, CompCodeImmediate)
    , m_tap(tap)
  {
  }
  const TermActionPair &getTermActionPair() const override {
    return m_tap;
  }
  String toString() const override;
};
