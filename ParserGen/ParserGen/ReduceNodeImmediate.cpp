#include "stdafx.h"
#include "Grammar.h"
#include "ReduceNodeImmediate.h"

String ReduceNodeImmediate::toString() const {
  const String result = __super::toString() + indentString(m_tap.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}
