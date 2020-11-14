#include "stdafx.h"
#include "Grammar.h"

TermSet ActionArray::getLookaheadSet(UINT terminalCount) const {
  TermSet result(terminalCount);
  for(const ParserAction pa : *this) {
    result.add(pa.m_token);
  }
  return result;
}

NTindexSet ActionArray::getNTindexSet(UINT terminalCount, UINT symbolCount) const {
  NTindexSet result(symbolCount - terminalCount);
  for(const ParserAction &su : *this) {
    result.add((size_t)su.m_token - terminalCount);
  }
  return result;
}

RawActionArray ActionArray::getRawActionArray() const {
  RawActionArray result(size());
  for(const ParserAction pa : *this) {
    result.add(pa.m_action);
  }
  return result;
}

SuccessorArray ActionArray::getSuccessorArray() const {
  SuccessorArray result(size());
  for(const ParserAction &su : *this) {
    result.add(su.m_action);
  }
  return result;
}

String ActionArray::toString(const SymbolNameContainer &symbolNames, bool actions) const {
  String result;
  for(const ParserAction pa : *this) {
    result += format(_T("   %s\n"), pa.toString(symbolNames, actions).cstr());
  }
  return result;
}

String ParserAction::toString(const SymbolNameContainer &symbolNames, bool action) const {
  const TCHAR *name = symbolNames.getSymbolName(m_token);
  if(action) {
    if(m_action > 0) {        // shiftItem
      return format(_T("shift to %d on %s" ),  m_action, name);
    } else if(m_action < 0) { // reduceItem
      return format(_T("reduce by %d on %s"), -m_action, name);
    } else {                  // accept
      return format(_T("accept on %s"      ),            name);
    }
  } else { // succesor
    return format(_T("goto %d on %s"       ),  m_action, name);
  }
}
