#include "stdafx.h"
#include "SymbolNodeBinSearch.h"

String SymbolNodeBinSearch::toString() const {
  const String result = __super::toString()  + indentString(m_statePairArray.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}
