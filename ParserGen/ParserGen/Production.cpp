#include "stdafx.h"
#include "Grammar.h"

Production::Production(const Grammar *grammar, const Production &prod)
: m_grammar(*grammar)
, m_leftSide(prod.m_leftSide)
, m_rightSide(prod.m_rightSide)
, m_actionBody(prod.m_actionBody)
, m_precedence(prod.m_precedence)
, m_pos(prod.m_pos)
{
}

bool Production::canTerminate() const {
  const UINT length = getLength();
  for(UINT s = 0; s < length; s++) {
    if(!m_grammar.getSymbol(m_rightSide[s]).m_terminate) {
      return false;
    }
  }
  return true;
}

bool Production::deriveEpsilon() const {
  const UINT length = getLength();
  for(UINT s = 0; s < length; s++) {
    if(!m_grammar.getSymbol(m_rightSide[s]).m_deriveEpsilon) {
      return false;
    }
  }
  return true;
}

String Production::toString() const {
  return m_grammar.getSymbolName(m_leftSide)
       + _T(" -> ")
       + getRightSideString();
}

String Production::getRightSideString() const {
  const UINT length = getLength();
  if(length == 0) {
    return _T("epsilon");
  }
  String result;
  for(UINT i = 0; i < length; i++) {
    const UINT s = m_rightSide[i];
    if(i > 0) {
      result += ' ';
    }
    result += m_rightSide[i].toString(m_grammar);
  }
  return result;
}
