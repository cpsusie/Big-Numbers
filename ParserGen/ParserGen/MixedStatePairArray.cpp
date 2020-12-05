#include "stdafx.h"
#include "MixedStatePairArray.h"

MixedStatePairArray::MixedStatePairArray(const SymbolNodeBase &base, const StatePairArray &statePairArray)
: SymbolNodeBase(base)
{
  CompactUIntHashMap<UINT, 256>  sameNewStateMap(241); // map from newState -> index into m_statePairBitSetArray
  for(const StatePair sp : statePairArray) {
    const UINT  newState = sp.m_newState;
    const UINT *indexp   = sameNewStateMap.get(newState);
    if(indexp) {
      m_statePairBitSetArray[*indexp].addFromState(sp.m_fromState);
    } else {
      const UINT index = (UINT)m_statePairBitSetArray.size();
      m_statePairBitSetArray.add(StatePairBitSet(getGrammar(), newState, sp.m_fromState, sp.m_shiftText));
      sameNewStateMap.put(newState, index);
    }
  }
  m_statePairBitSetArray.sortBySetSize();
}

StatePairArray MixedStatePairArray::mergeAll() const {
  StatePairArray result(getFromStateCount());
  result.addAll(m_statePairArray);
  if(!m_statePairBitSetArray.isEmpty()) {
    for(auto it = m_statePairBitSetArray.getIterator(); it.hasNext();) {
      result.addAll((StatePairArray)it.next());
    }
    if(result.size() > 1) {
      result.sortByFromState();
    }
  }
  return result;
}
