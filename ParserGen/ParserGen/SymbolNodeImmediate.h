#pragma once

#include "StatePair.h"
#include "SymbolNode.h"

class SymbolNodeImmediate : public SymbolNode {
private:
  const StatePair m_statePair;
public:
  SymbolNodeImmediate(const SymbolNode *parent, const SymbolNodeBase &base, const StatePair &statePair)
    : SymbolNode(parent, base, 1, CompCodeImmediate)
    , m_statePair(statePair)
  {
  }
  SymbolNodeImmediate(const SymbolNode *parent, const SymbolNodeBase &base, UINT newState, UINT fromStateCount)
    : SymbolNode(parent, base, fromStateCount, CompCodeImmediate)
    , m_statePair(StatePair(StatePair::NoFromStateCheck, newState, false, fromStateCount))
  {
  }
  bool isDontCareNode() const override {
    return m_statePair.isNoFromStateCheck();
  }
  const StatePair &getStatePair() const override {
    return m_statePair;
  }
  String toString() const override;
};
