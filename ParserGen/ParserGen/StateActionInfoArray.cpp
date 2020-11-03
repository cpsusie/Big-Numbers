#include "stdafx.h"
#include "StateActionInfoArray.h"

StateActionInfoArray::StateActionInfoArray(const GrammarTables &tables)
: m_symbolNameArray(tables.getSymbolNameArray())
{
  const UINT stateCount    = tables.getStateCount();
  const UINT terminalCount = tables.getTerminalCount();
  setCapacity(stateCount);
  const Array<ActionArray> &stateActions = tables.getStateActions();
  for(UINT state = 0; state < stateCount; state++) {
    add(new StateActionInfo(terminalCount, state, stateActions[state], *this));
  }
//  redirectDebugLog();
//  debugLog(_T("%s"), toString().cstr());
}

StateActionInfoArray::~StateActionInfoArray() {
  clear();
}

void StateActionInfoArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    delete (*this)[i];
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
