#include "stdafx.h"
#include "Grammar.h"

TermSet ParserActionArray::getLegalTermSet(UINT terminalCount) const {
  TermSet result(terminalCount);
  for(const ParserAction pa : *this) {
    result.add(pa.m_term);
  }
  return result;
}

ActionArray ParserActionArray::getActionArray() const {
  ActionArray result(size());
  for(const ParserAction pa : *this) {
    result.add(pa.m_action);
  }
  return result;
}

String ParserActionArray::toString(const SymbolNameContainer &symbolNames) const {
  String result;
  for(const ParserAction pa : *this) {
    result += format(_T("   %s\n"), pa.toString(symbolNames).cstr());
  }
  return result;
}

String ParserAction::toString(const SymbolNameContainer &symbolNames) const {
  const TCHAR *name = symbolNames.getSymbolName(m_term);
  if(m_action > 0) {        // shiftItem
    return format(_T("shift to %d on %s" ),  m_action, name);
  } else if(m_action < 0) { // reduceItem
    return format(_T("reduce by %d on %s"), -m_action, name);
  } else {                  // accept
    return format(_T("accept on %s"      ),            name);
  }
}
