#include "stdafx.h"
#include "Grammar.h"

SymbolSet ActionArray::getLookaheadSet(UINT terminalCount) const {
  SymbolSet result(terminalCount);
  for(const ParserAction pa : *this) {
    result.add(pa.m_token);
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
