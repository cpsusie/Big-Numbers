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

String ParserActionArray::toString(const SymbolNameContainer &nameContainer) const {
  String result;
  for(const ParserAction pa : *this) {
    result += format(_T("   %s\n"), pa.toString(nameContainer).cstr());
  }
  return result;
}

String ParserAction::toString(const SymbolNameContainer &nameContainer) const {
  const TCHAR *name = nameContainer.getSymbolName(m_term).cstr();
  if(m_action > 0) {        // shiftItem
    return format(_T("shift to %d on %s" ),  m_action, name);
  } else if(m_action < 0) { // reduceItem
    return format(_T("reduce by %d on %s"), -m_action, name);
  } else {                  // accept
    return format(_T("accept on %s"      ),            name);
  }
}
