#include "stdafx.h"
#include "Grammar.h"
#include "SymbolNodeBase.h"

UINT SymbolNodeBase::getNTIndex() const {
  return getGrammar().NTermToNTIndex(m_symbol);
}

UINT SymbolNodeBase::getTerm() const {
  assert(isTerminal());
  return m_symbol;
}

bool SymbolNodeBase::isTerminal() const {
  return getGrammar().isTerminal(m_symbol);
}

const String &SymbolNodeBase::getSymbolName() const {
  return getGrammar().getSymbolName(m_symbol);
}
