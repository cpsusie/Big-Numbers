#include "stdafx.h"
#include "SymbolNodeSplit.h"

SymbolNodeSplit::~SymbolNodeSplit() {
  SAFEDELETE(m_child[0]);
  SAFEDELETE(m_child[1]);
}

SymbolNodeSplit &SymbolNodeSplit::setChild(BYTE index, SymbolNode *child) {
  assert(index < 2);
  assert(m_child[index] == nullptr);
  m_child[index] = child;
  return *this;
}

void SymbolNodeSplit::addAllBitSets(BitSetArray &a) const {
  getChild(0).addAllBitSets(a);
  getChild(1).addAllBitSets(a);
}

String SymbolNodeSplit::toString() const {
  const String cstr0  = indentString(getChild(0).toString(),2);
  const String cstr1  = indentString(getChild(1).toString(),2);
  const String result = __super::toString() + indentString(format(_T("Child 0:\n%s\nChild 1:\n%s"), cstr0.cstr(), cstr1.cstr()), 4);
  return indentString(result, m_recurseLevel * 2);
}
