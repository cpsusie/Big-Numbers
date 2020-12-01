#include "stdafx.h"
#include "StatePairMatrix.h"

TransposedSuccessorMatrix::TransposedSuccessorMatrix(const Grammar &grammar) : StatePairMatrix(grammar, false, OPTPARAM_SUCC) {
  const UINT           termCount  = grammar.getTermCount();
  const UINT           ntermCount = grammar.getNTermCount();
  const UINT           stateCount = grammar.getStateCount();
  const GrammarResult &r          = grammar.getResult();

  for(UINT state = 0; state < stateCount; state++) {
    const SuccessorStateArray &succArray = r.m_stateResult[state].m_succs;
    const UINT                 n         = succArray.getLegalNTermCount();
    if(n > 0) {
      for(SuccessorState succ : succArray) {
        (*this)[succ.m_nterm - termCount].add(StatePair(state, succ.m_newState, false));
      }
    }
  }
  sortAllRows();
}

