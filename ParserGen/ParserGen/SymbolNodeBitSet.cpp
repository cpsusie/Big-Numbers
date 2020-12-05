#include "stdafx.h"
#include "SymbolNodeBitSet.h"

String SymbolNodeBitSet::toString() const {
  const String result = __super::toString() + indentString(m_statePairBitSet.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}
