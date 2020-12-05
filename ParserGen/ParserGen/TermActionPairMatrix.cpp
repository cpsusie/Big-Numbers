#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"
#include "TermActionPairMatrix.h"

TermActionPairMatrix::TermActionPairMatrix(const Grammar &grammar, BYTE selectFlags) : Array(grammar.getStateCount()) {
  UINT state = 0;
  switch(selectFlags) {
  case 0                   :
    { const UINT stateCount = grammar.getStateCount();
      for(UINT s = 0; s < stateCount; s++) {
        add(TermActionPairArray());
      }
    }
    break;
  case SELECT_SHIFTACTIONS :
    { for(auto it = grammar.getResult().m_stateResult.getIterator(); it.hasNext(); state++) {
        const StateResult &sr = it.next();
        assert(sr.m_index == state);
        add(sr.m_termActionArray.selectShiftTermActionPairs());
      }
    }
    break;
  case SELECT_REDUCEACTIONS:
    { for(auto it = grammar.getResult().m_stateResult.getIterator(); it.hasNext(); state++) {
        const StateResult &sr = it.next();
        assert(sr.m_index == state);
        add(sr.m_termActionArray.selectReduceTermActionPairs());
      }
    }
    break;
  case SELECT_ALLACTIONS   :
    { for(auto it = grammar.getResult().m_stateResult.getIterator(); it.hasNext(); state++) {
        const StateResult &sr = it.next();
        assert(sr.m_index == state);
        add(sr.m_termActionArray);
      }
    }
    break;
  default                  :
    throwInvalidArgumentException(__TFUNCTION__, _T("selectFlags=%#04x"), selectFlags);
  }
}
