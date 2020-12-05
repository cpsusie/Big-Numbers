#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"
#include "NTermNewStatePairMatrix.h"

NTermNewStatePairMatrix::NTermNewStatePairMatrix(const Grammar &grammar) : Array(grammar.getStateCount()) {
  for(auto it = grammar.getResult().m_stateResult.getIterator(); it.hasNext();) {
    add(it.next().m_ntermNewStateArray);
  }
}

