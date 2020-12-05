#include "stdafx.h"
#include "StatePair.h"

StatePair::StatePair(UINT fromState, UINT newState, bool shiftText, UINT fromStateCount)
  : m_fromState(     fromState     )
  , m_newState(      newState      )
  , m_shiftText(     shiftText     )
  , m_fromStateCount(fromStateCount)
{
  if(m_shiftText) {
    assert(!isNoFromStateCheck() && (fromStateCount == 1));
  } else {
    assert(isNoFromStateCheck() || (fromStateCount == 1));
  }
  assert(m_newState != NoFromStateCheck);
}

String StatePair::toString() const {
  return isNoFromStateCheck()
       ? format(_T("Shift to %4u No check (%s)"), m_newState, ElementName::createElementCountText(ETYPE_STATE, getFromStateCount()).cstr())
       : format(_T("Shift to %4u from %4u"), m_newState, m_fromState);
}
