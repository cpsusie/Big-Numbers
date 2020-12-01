#include "stdafx.h"
#include "StatePairMatrix.h"

StatePairMatrix::StatePairMatrix(const Grammar &grammar, bool rowsIndexedByTerminal, OptimizationParameters optParam)
  : m_grammar(grammar                            )
  , m_rowsIndexedByTerminal(rowsIndexedByTerminal)
  , m_optParam(optParam                          )
{
  const UINT rowCount = m_rowsIndexedByTerminal ? grammar.getTermCount() : grammar.getNTermCount();
  setCapacity(rowCount);
  for(UINT r = 0; r < rowCount; r++) {
    add(StatePairArray());
  }
}

void StatePairMatrix::sortAllRows() {
  for(auto it = getIterator(); it.hasNext();) {
    it.next().sortByNewState();
  }
}

UINT StatePairMatrix::rowToSymbol(UINT row) const {
  return m_rowsIndexedByTerminal ? row : (row + m_grammar.getTermCount());
}

UINT StatePairMatrix::symbolToRow(UINT symbol) const {
  if(m_rowsIndexedByTerminal) {
    return symbol;
  } else {
    const UINT termCount = m_grammar.getTermCount();
    assert(symbol >= termCount);
    return symbol - termCount;
  }
}

