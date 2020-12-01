#include "stdafx.h"
#include "Grammar.h"

NTIndexSet SuccessorStateArray::getNTIndexSet(UINT termCount, UINT symbolCount) const {
  NTIndexSet result(symbolCount - termCount);
  for(const SuccessorState &ss : *this) {
    result.add((size_t)ss.m_nterm - termCount);
  }
  return result;
}

StateArray SuccessorStateArray::getStateArray() const {
  StateArray result(size());
  for(const SuccessorState &ss : *this) {
    result.add(ss.m_newState);
  }
  return result;
}

String SuccessorStateArray::toString(const AbstractSymbolNameContainer &nameContainer) const {
  String result;
  for(const SuccessorState ss : *this) {
    result += format(_T("   %s\n"), ss.toString(nameContainer).cstr());
  }
  return result;
}

String SuccessorState::toString(const AbstractSymbolNameContainer &nameContainer) const {
  const TCHAR *name = nameContainer.getSymbolName(m_nterm).cstr();
  return format(_T("goto %u on %s"), m_newState, name);
}

SuccessorMatrix::SuccessorMatrix(const Grammar &grammar) : Array(grammar.getStateCount()) {
  for(auto it = grammar.getResult().m_stateResult.getIterator(); it.hasNext();) {
    add(it.next().m_succs);
  }
}
