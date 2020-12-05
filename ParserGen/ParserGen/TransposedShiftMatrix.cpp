#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"
#include "StatePairMatrix.h"

TransposedShiftMatrix::TransposedShiftMatrix(const Grammar &grammar) : StatePairMatrix(grammar, true, OPTPARAM_SHIFT) {
  const UINT           termCount  = grammar.getTermCount();
  const UINT           stateCount = grammar.getStateCount();
  const GrammarResult &r          = grammar.getResult();

  for(UINT state = 0; state < stateCount; state++) {
    const TermActionPairArray &termActionArray = r.m_stateResult[state].m_termActionArray;
    for(TermActionPair tap : termActionArray) {
      if(tap.isShiftAction()) {
        (*this)[tap.getTerm()].add(StatePair(state, tap.getNewState(), true));
      }
    }
  }
  sortAllRows();
}
