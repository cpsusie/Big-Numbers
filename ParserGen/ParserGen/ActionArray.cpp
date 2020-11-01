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
