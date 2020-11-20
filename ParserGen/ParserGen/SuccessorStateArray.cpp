#include "stdafx.h"
#include "Grammar.h"

NTindexSet SuccessorStateArray::getNTindexSet(UINT termCount, UINT symbolCount) const {
  NTindexSet result(symbolCount - termCount);
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

String SuccessorStateArray::toString(const SymbolNameContainer &nameContainer) const {
  String result;
  for(const SuccessorState ss : *this) {
    result += format(_T("   %s\n"), ss.toString(nameContainer).cstr());
  }
  return result;
}

String SuccessorState::toString(const SymbolNameContainer &nameContainer) const {
  const TCHAR *name = nameContainer.getSymbolName(m_nterm).cstr();
  return format(_T("goto %u on %s"), m_newState, name);
}
