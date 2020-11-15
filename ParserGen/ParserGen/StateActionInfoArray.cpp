#include "stdafx.h"
#include "StateActionInfoArray.h"

StateActionInfoArray::StateActionInfoArray(const GrammarTables &tables)
: m_tables(tables)
{
  const UINT           stateCount    = tables.getStateCount();
  const UINT           terminalCount = tables.getTerminalCount();
  const ActionMatrix  &am            = tables.getActionMatrix();
  setCapacity(stateCount);

  for(UINT state = 0; state < stateCount; state++) {
    const StateActionInfo *info = new StateActionInfo(state, am[state], tables); TRACE_NEW(info);
    add(info);
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
