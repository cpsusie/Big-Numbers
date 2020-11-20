#include "stdafx.h"
#include "StateActionNodeArray.h"

namespace ActionMatrixCompression {

StateActionNodeArray::StateActionNodeArray(const GrammarTables &tables)
: m_tables(tables)
{
  const UINT           stateCount = tables.getStateCount();
  const UINT           termCount  = tables.getTermCount();
  const ActionMatrix  &am         = tables.getActionMatrix();
  setCapacity(stateCount);
//  redirectDebugLog();
  for(UINT state = 0; state < stateCount; state++) {
    add(StateActionNode::allocateStateActionNode(state, tables, am[state]));
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
