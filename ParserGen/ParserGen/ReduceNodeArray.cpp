#include "stdafx.h"
#include "TermActionPairMatrix.h"
#include "ReduceNodeArray.h"

ReduceNodeArray::ReduceNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt)
: m_grammar(grammar)
{
  const TermActionPairMatrix rm(grammar, TermActionPairMatrix::SELECT_REDUCEACTIONS);
  const UINT         rowCount = rm.getRowCount();
  setCapacity(rowCount);
  for(UINT r = 0; r < rowCount; r++) {
    const TermActionPairArray &row = rm[r];
    add(row.isEmpty() ? nullptr : ReduceNode::allocateReduceNode(grammar, r, row, opt));
  }
  if(Options::getInstance().m_verboseLevel >= 3) {
    verbose(3, _T("%s"), toString().cstr());
  }
}

ReduceNodeArray::~ReduceNodeArray() {
  clear();
}

void ReduceNodeArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

String ReduceNodeArray::getNullString(UINT index) const {
  return format(_T("0 State %4u - null\n"), index);
}

String ReduceNodeArray::toString() const {
  String result;
  UINT   index = 0;
  for(auto it = getIterator(); it.hasNext(); index++) {
    const ReduceNode *n = it.next();
    result += n ? n->toString() : getNullString(index);
  }
  return result;
}
