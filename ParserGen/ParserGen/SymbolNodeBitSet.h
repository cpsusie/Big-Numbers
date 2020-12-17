#pragma once

#include "StatePairBitSet.h"
#include "SymbolNode.h"

class SymbolNodeBitSet : public SymbolNode {
private:
  const StatePairBitSet m_statePairBitSet;
public:
  SymbolNodeBitSet(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairBitSet &statePairBitSet)
    : SymbolNode(parent, base, statePairBitSet.getFromStateCount(), CompCodeBitSet)
    , m_statePairBitSet(statePairBitSet)
  {
  }
  const StatePairBitSet &getStatePairBitSet() const final {
    return m_statePairBitSet;
  }
  void addAllBitSets(BitSetArray &a) const final {
    a.add(m_statePairBitSet.getFromStateSet());
  }
  String toString() const final;
};
