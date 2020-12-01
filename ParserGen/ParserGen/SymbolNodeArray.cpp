#include "stdafx.h"
#include "StatePairMatrix.h"
#include "SymbolNodeArray.h"

SymbolNodeArray::SymbolNodeArray(const StatePairMatrix &spm) : m_grammar(spm.getGrammar()) {
//  redirectDebugLog();
//  debugLog(_T("%s"), tsm.toString(_T("\n"), BT_NOBRACKETS).cstr());

  const MatrixOptimizeParameters &optParam = spm.getOptimizeParameters();
  const UINT                      rowCount = spm.getRowCount();
  setCapacity(spm.getRowCount());
  for(UINT r = 0; r < rowCount; r++) {
    const StatePairArray &row = spm[r];
    add(row.isEmpty() ? nullptr : SymbolNode::allocateSymbolNode(m_grammar, spm.rowToSymbol(r), row, optParam));
//    debugLog(_T("%s"), (last() == nullptr) ? _T("null\n") : last()->toString().cstr());
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

String SymbolNodeArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}
