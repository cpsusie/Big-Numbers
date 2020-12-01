#include "stdafx.h"
#include "StatePairMatrix.h"

TransposedShiftMatrix::TransposedShiftMatrix(const Grammar &grammar) : StatePairMatrix(grammar, true, OPTPARAM_SHIFT) {
  const UINT           termCount  = grammar.getTermCount();
  const UINT           stateCount = grammar.getStateCount();
  const GrammarResult &r          = grammar.getResult();

  for(UINT state = 0; state < stateCount; state++) {
    const ParserActionArray &actionArray = r.m_stateResult[state].m_actions;
    for(ParserAction pa : actionArray) {
      if(pa.isShift()) {
        (*this)[pa.m_term].add(StatePair(state, pa.m_action, true));
      }
    }
  }
  sortAllRows();
}
