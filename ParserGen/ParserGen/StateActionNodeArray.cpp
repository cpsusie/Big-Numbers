#include "stdafx.h"
#include "StateActionNodeArray.h"

namespace ActionMatrixCompression {

StateActionNodeArray::StateActionNodeArray(const Grammar &grammar, const MatrixOptimizeParameters &opt)
: m_grammar(grammar)
{
  const UINT          stateCount = grammar.getStateCount();
  const ActionMatrix  actionMatrix(grammar);
  setCapacity(stateCount);
//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    add(StateActionNode::allocateStateActionNode(grammar, state, actionMatrix[state], opt));
//    debugLog(_T("%s"), last()->toString().cstr());
  }
}

StateActionNodeArray::~StateActionNodeArray() {
  clear();
}

void StateActionNodeArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

String StateActionNodeArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}

}; // namespace ActionMatrixCompression
