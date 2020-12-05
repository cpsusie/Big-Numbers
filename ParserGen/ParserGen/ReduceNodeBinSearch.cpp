#include "stdafx.h"
#include "Grammar.h"
#include "ReduceNodeBinSearch.h"

String ReduceNodeBinSearch::toString() const {
  const String result = __super::toString()  + indentString(m_termActionArray.toString(getGrammar()),3);
  return indentString(result, m_recurseLevel * 2);
}
