#include "stdafx.h"
#include "ReduceNodeSplit.h"

ReduceNodeSplit::~ReduceNodeSplit() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

ReduceNodeSplit &ReduceNodeSplit::setChild(BYTE index, ReduceNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

String ReduceNodeSplit::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 3);
  return indentString(result, m_recurseLevel * 2);
}
