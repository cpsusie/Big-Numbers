#include "stdafx.h"
#include "ReduceNodeBitSet.h"

String ReduceNodeBitSet::toString() const {
  const String result = __super::toString() + indentString(m_termSetReduction.toString(),3);
  return indentString(result, m_recurseLevel * 2);
}
