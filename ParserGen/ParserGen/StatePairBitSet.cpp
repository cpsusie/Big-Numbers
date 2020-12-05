#include "stdafx.h"
#include "Grammar.h"
#include "StatePairArray.h"
#include "StatePairBitSet.h"

StatePairBitSet::StatePairBitSet(const Grammar &grammar, UINT newState, UINT fromState0, bool shiftText)
  : m_grammar(         grammar                )
  , m_fromStateSet(    grammar.getStateCount())
  , m_fromStateCount(  0                      )
  , m_newState(        newState               )
  , m_shiftText(       shiftText              )
{
  addFromState(fromState0);
}

StatePairBitSet::operator  StatePairArray() const {
  StatePairArray      result(getFromStateCount());

  for(auto it = m_fromStateSet.getIterator(); it.hasNext();) {
    result.add(StatePair((USHORT)it.next(), m_newState, m_shiftText));
  }
  return result;
}

String StatePairBitSet::toString()  const {
  return format(_T("Shift to %4u from %s (%u states)")
               ,m_newState
               ,m_fromStateSet.toRangeString().cstr()
               ,getFromStateCount()
               );
}
