#include "stdafx.h"
#include "SymbolNodeImmediate.h"

String SymbolNodeImmediate::toString() const {
  const String result = __super::toString() + indentString(m_statePair.toString(), 4);
  return indentString(result, m_recurseLevel * 2);
}
