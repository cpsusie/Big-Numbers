#include "stdafx.h"
#include "Grammar.h"

NTindexSet SuccessorStateArray::getNTindexSet(UINT terminalCount, UINT symbolCount) const {
  NTindexSet result(symbolCount - terminalCount);
  for(const SuccessorState &ss : *this) {
    result.add((size_t)ss.m_nt - terminalCount);
  }
  return result;
}

SuccessorArray SuccessorStateArray::getSuccessorArray() const {
  SuccessorArray result(size());
  for(const SuccessorState &ss : *this) {
    result.add(ss.m_newState);
  }
  return result;
}

String SuccessorStateArray::toString(const SymbolNameContainer &nameContainer) const {
  String result;
  for(const SuccessorState ss : *this) {
    result += format(_T("   %s\n"), ss.toString(nameContainer).cstr());
  }
  return result;
}

String SuccessorState::toString(const SymbolNameContainer &nameContainer) const {
  const TCHAR *name = nameContainer.getSymbolName(m_nt);
  return format(_T("goto %u on %s"), m_newState, name);
}
