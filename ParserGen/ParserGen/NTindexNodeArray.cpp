#include "stdafx.h"
#include "NTindexNodeArray.h"

namespace TransposedSuccessorMatrixCompression {

// rows indexed by NTindex, has variable length. Each elment in a row is a StatePair containing m_state,m_newState
// elements ordered by m_newState
class TransposedSuccessorMatrix : public Array<StatePairArray> {
public:
  TransposedSuccessorMatrix(const Grammar &grammar);
};

TransposedSuccessorMatrix::TransposedSuccessorMatrix(const Grammar &grammar) {
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
    const UINT                 n         = succArray.getLegalNTermCount();
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

NTindexNodeArray::NTindexNodeArray(const Grammar &grammar) : m_grammar(grammar) {
  const TransposedSuccessorMatrix tsm(grammar);

//  redirectDebugLog();
//  debugLog(_T("%s"), tsm.toString(_T("\n"), BT_NOBRACKETS).cstr());

  const UINT NTermCount = grammar.getNTermCount();
  setCapacity(NTermCount);
  for(UINT NTindex = 0; NTindex < NTermCount; NTindex++) {
    const StatePairArray &row = tsm[NTindex];
    add(row.isEmpty() ? nullptr : NTindexNode::allocateNTindexNode(NTindex, grammar, tsm[NTindex]));
//    debugLog(_T("%s"), (last() == nullptr) ? _T("null\n") : last()->toString().cstr());
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

}; // namespace TransposedSuccessorMatrixCompression
