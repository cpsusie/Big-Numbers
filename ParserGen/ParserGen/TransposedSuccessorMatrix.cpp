#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"
#include "StatePairMatrix.h"

TransposedSuccessorMatrix::TransposedSuccessorMatrix(const Grammar &grammar) : StatePairMatrix(grammar, false, OPTPARAM_SUCC) {
  const UINT           termCount  = grammar.getTermCount();
  const UINT           ntermCount = grammar.getNTermCount();
  const UINT           stateCount = grammar.getStateCount();
  const GrammarResult &r          = grammar.getResult();

  for(UINT state = 0; state < stateCount; state++) {
    const NTermNewStatePairArray &ntermNewStateArray = r.m_stateResult[state].m_ntermNewStateArray;
    const UINT                    n                  = ntermNewStateArray.getLegalNTermCount();
    if(n > 0) {
      for(NTermNewStatePair ntns : ntermNewStateArray) {
        (*this)[grammar.NTermToNTIndex(ntns.getNTerm())].add(StatePair(state, ntns.getNewState(), false));
      }
    }
  }
  sortAllRows();
}

