#include "stdafx.h"
#include "ReduceNodeArray.h"

namespace TransposedShiftMatrixCompression {

ReduceNodeArray::ReduceNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt)
: m_grammar(grammar)
{
  const UINT          stateCount = grammar.getStateCount();
  const ActionMatrix  actionMatrix(grammar);
  setCapacity(stateCount);
//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    add(ReduceNode::allocateReduceNode(grammar, state, actionMatrix[state], opt));
//    debugLog(_T("%s"), last()->toString().cstr());
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

String ReduceNodeArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}

}; // namespace TransposedShiftMatrixCompression
