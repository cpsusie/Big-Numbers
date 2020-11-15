#include "stdafx.h"
#include "StateActionInfoArray.h"

StateActionInfoArray::StateActionInfoArray(const GrammarTables &tables)
: m_tables(tables)
{
  const UINT           stateCount    = tables.getStateCount();
  const UINT           terminalCount = tables.getTerminalCount();
  const ActionMatrix  &am            = tables.getActionMatrix();
  setCapacity(stateCount);
//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    add(StateActionInfo::allocateStateActionInfo(state, tables, am[state]));
//    debugLog(_T("%s"), last()->toString().cstr());
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
