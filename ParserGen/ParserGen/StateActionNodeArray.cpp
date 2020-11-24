#include "stdafx.h"
#include "StateActionNodeArray.h"

namespace ActionMatrixCompression {

StateActionNodeArray::StateActionNodeArray(const Grammar &grammar)
: m_grammar(grammar)
{
  const UINT           stateCount = grammar.getStateCount();
  ActionMatrix         actionMatrix;
  grammar.getResult().getActionMatrix(actionMatrix);
  setCapacity(stateCount);
//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    add(StateActionNode::allocateStateActionNode(state, grammar, actionMatrix[state]));
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
