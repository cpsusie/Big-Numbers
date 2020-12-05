#pragma once

#include "SymbolNodeBase.h"
#include "StatePairBitSetArray.h"
#include "StatePairArray.h"

class MixedStatePairArray : public SymbolNodeBase {
public:
  StatePairArray        m_statePairArray;
  StatePairBitSetArray  m_statePairBitSetArray;
  MixedStatePairArray(const SymbolNodeBase &base, const StatePairArray &statePairArray);
  StatePairArray        mergeAll()          const;
  inline UINT           getFromStateCount() const {
    return m_statePairArray.getFromStateCount() + m_statePairBitSetArray.getFromStateCount();
  }
  inline UINT           getNewStateCount()  const {
    return m_statePairArray.getNewStateCount() + m_statePairBitSetArray.getNewStateCount();
  }
  MixedStatePairArray   &removeFirstBitSet() {
    m_statePairBitSetArray.removeIndex(0);
    return *this;
  }
};
