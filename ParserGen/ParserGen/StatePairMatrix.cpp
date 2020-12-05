#include "stdafx.h"
#include "Grammar.h"
#include "StatePairMatrix.h"

StatePairMatrix::StatePairMatrix(const Grammar &grammar, bool rowsIndexedByTerminal, OptimizationParameters optParam)
  : IndexToSymbolConverter(grammar, rowsIndexedByTerminal)
  , m_grammar(grammar                            )
  , m_optParam(optParam                          )
{
  const UINT n = getIndexCount();
  setCapacity(n);
  for(UINT i = 0; i < n; i++) {
    add(StatePairArray());
  }
}

void StatePairMatrix::sortAllRows() {
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

