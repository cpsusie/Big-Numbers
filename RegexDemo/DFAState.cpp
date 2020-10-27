#include "stdafx.h"
#include "DFA.h"

#if defined(_DEBUG)

void DFAState::getTransitionMap(DFATransitionMap &map) const {
  int           lastTransition = FAILURE;
  CharacterSet *currentSet     = nullptr;
  map.clear();

  for(int ch = 0; ch < MAX_CHARS; ch++) {
    if(m_transition[ch] != FAILURE) {
      if(m_transition[ch] != lastTransition) {
        lastTransition = m_transition[ch];
        currentSet = map.get(lastTransition);
        if(currentSet == nullptr) {
          map.put(lastTransition, CharacterSet());
        }
        currentSet = map.get(lastTransition);
      }
      currentSet->add(ch);
    }
  }
}

String DFAState::toString(bool showNFASet) const {
  String result = format(_T("DFA State %3d %s"), m_id, m_accept.toString().cstr());
  if(showNFASet) {
    result += format(_T("\nNFASet:%s"), m_NFAset.toString().cstr());
  }

  DFATransitionMap map;;
  getTransitionMap(map);
  for(Iterator<Entry<int, CharacterSet> > it = map.getIterator(); it.hasNext();) {
    const Entry<int, CharacterSet> &e = it.next();
    result += format(_T("\n   goto %2d on %s"), e.getKey(), charBitSetToString(e.getValue(), NFAState::getFormater()).cstr());
  }
  result += _T("\n");
  return result;
}

#endif
