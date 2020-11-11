#include "stdafx.h"
#include "StateActionInfoArray.h"

StateActionInfoArray::StateActionInfoArray(const GrammarTables &tables)
: m_symbolNameArray(tables.getSymbolNameArray())
{
  const UINT                stateCount    = tables.getStateCount();
  const UINT                terminalCount = tables.getTerminalCount();
  const Array<ActionArray> &stateActions  = tables.getStateActions();
  setCapacity(stateCount);

//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    const StateActionInfo *info = new StateActionInfo(terminalCount, state, stateActions[state], *this); TRACE_NEW(info);
    add(info);
//    debugLog(_T("%s\n"), last()->toString().cstr());
  }
}

StateActionInfoArray::~StateActionInfoArray() {
  clear();
}

void StateActionInfoArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

String StateActionInfoArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}
