#include "stdafx.h"
#include "NTindexNodeArray.h"

namespace TransSuccMatrixCompression {

NTindexNodeArray::NTindexNodeArray(const Grammar &grammar) : m_grammar(grammar) {
  TransposeSuccessorMatrix tsm(grammar);

  const UINT NTermCount = grammar.getNTermCount();
  setCapacity(NTermCount);
  redirectDebugLog();
  for(UINT NTindex = 0; NTindex < NTermCount; NTindex++) {
    const StatePairArray &row = tsm[NTindex];
    add(row.isEmpty() ? nullptr : NTindexNode::allocateNTindexNode(NTindex, grammar, tsm[NTindex]));
    debugLog(_T("%s"), (last() == nullptr) ? _T("null\n") : last()->toString().cstr());
  }
}

NTindexNodeArray::~NTindexNodeArray() {
  clear();
}

void NTindexNodeArray::clear() {
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    SAFEDELETE((*this)[i]);
  }
  __super::clear();
}

String NTindexNodeArray::toString() const {
  String result;
  for(auto it = getIterator(); it.hasNext();) {
    result += it.next()->toString();
    result += '\n';
  }
  return result;
}

TransposeSuccessorMatrix::TransposeSuccessorMatrix(const Grammar &grammar) {
  const UINT           termCount  = grammar.getTermCount();
  const UINT           NTermCount = grammar.getNTermCount();
  const UINT           stateCount = grammar.getStateCount();
  const GrammarResult &r          = grammar.getResult();

  setCapacity(NTermCount);
  for(UINT ntIndex = 0; ntIndex < NTermCount; ntIndex++) {
    add(StatePairArray());
  }
  for(UINT state = 0; state < stateCount; state++) {
    const SuccessorStateArray &succArray = r.m_stateResult[state].m_succs;
    const UINT n = succArray.getLegalNTermCount();
    if(n > 0) {
      for(SuccessorState succ : succArray) {
        (*this)[succ.m_nterm - termCount].add(StatePair(state, succ.m_newState));
      }
    }
  }
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

}; // namespace TransSuccMatrixCompression
