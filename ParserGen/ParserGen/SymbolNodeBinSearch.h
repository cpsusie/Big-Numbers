#pragma once

#include "StatePairArray.h"
#include "SymbolNode.h"

class SymbolNodeBinSearch : public SymbolNode {
private:
  StatePairArray m_statePairArray;
public:
  SymbolNodeBinSearch(const SymbolNode *parent, const SymbolNodeBase &base, const StatePairArray &statePairArray)
    : SymbolNode(parent, base, statePairArray.getFromStateCount(), CompCodeBinSearch)
    , m_statePairArray(statePairArray)
  {
    assert(statePairArray.getFromStateCount() >= 2);
    m_statePairArray.sortByFromState();
  }
  const StatePairArray &getStatePairArray() const override {
    return m_statePairArray;
  }
  String toString() const override;
};
