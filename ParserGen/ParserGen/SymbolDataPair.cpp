#include "stdafx.h"
#include "TermActionPair.h"

  // Assume old symbol and term are both terminal-symbols and type is PA_SHIFT or PA_REDUCE
void SymbolDataPair::setTerm(UINT term, const AbstractSymbolNameContainer &nameContainer) {
  assert(nameContainer.isTerminal(term));
  assert(nameContainer.isTerminal(getSymbol()));
  assert(isValidActionType());
  m_symbol = term;
}

  // Assume type == PA_SHIFT or PA_NEWSTATE
void SymbolDataPair::setNewState(UINT newState) {
  assert(hasNewState());
  m_index = newState;
}

String SymbolDataPair::toString(const AbstractSymbolNameContainer &nameContainer) const {
  const String &name = nameContainer.getSymbolName(getSymbol());
  return format(_T("%s on %s"), getAction().toString().cstr(), name.cstr());
}
