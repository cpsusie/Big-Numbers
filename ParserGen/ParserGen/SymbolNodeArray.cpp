#include "stdafx.h"
#include "StatePairMatrix.h"
#include "Grammar.h"
#include "SymbolNodeArray.h"

SymbolNodeArray::SymbolNodeArray(const StatePairMatrix &spm)
  : m_grammar(spm.getGrammar())
  , IndexToSymbolConverter(spm)
{
  const MatrixOptimizeParameters &optParam = spm.getOptimizeParameters();
  const UINT                      n        = spm.getRowCount();
  setCapacity(spm.getRowCount());
  for(UINT i = 0; i < n; i++) {
    const StatePairArray &row = spm[i];
    add(row.isEmpty() ? nullptr : SymbolNode::allocateSymbolNode(m_grammar, indexToSymbol(i), row, optParam));
  }
  if(Options::getInstance().m_verboseLevel >= 3) {
    verbose(3, _T("%s"), toString().cstr());
  }
}

SymbolNodeArray::~SymbolNodeArray() {
  clear();
}

void SymbolNodeArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

BitSetArray &SymbolNodeArray::getBitSetArray(BitSetArray &a) const {
  a.clear();
  for(auto np : *this) {
    if(np == nullptr) continue;
    np->addAllBitSets(a);
  }
  return a;
}

String SymbolNodeArray::getElementName(UINT index) const {
  return m_grammar.getSymbolName(indexToSymbol(index));
}

String SymbolNodeArray::getNullString(UINT index) const {
  return format(_T("%u - %s null\n"), index, getElementName(index).cstr());
}

String SymbolNodeArray::toString() const {
  String result;
  UINT   index = 0;
  for(auto it = getIterator(); it.hasNext(); index++) {
    const SymbolNode *n = it.next();
    result += n ? n->toString() : getNullString(index);
  }
  return result;
}
