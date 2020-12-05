#pragma once

#include "StatePairArray.h"
#include "IndexToSymbolConverter.h"

class Grammar;
class StatePairMatrix : public Array<StatePairArray>, public IndexToSymbolConverter {
private:
  const Grammar               &m_grammar;
  const OptimizationParameters m_optParam;
protected:
  StatePairMatrix(const Grammar &grammar, bool rowsIndexedByTerminal, OptimizationParameters optParam);
  // sort all rows by newState
  void sortAllRows();
public:
  inline const Grammar &getGrammar() const {
    return m_grammar;
  }
  inline const MatrixOptimizeParameters &getOptimizeParameters() const {
    return Options::getOptParam(m_optParam);
  }
  inline UINT getRowCount() const {
    return (UINT)size();
  }
};

// rows indexed by term, has variable length. Each elment in a row is a StatePair containing m_state,m_newState
// elements ordered by m_newState
class TransposedShiftMatrix : public StatePairMatrix {
public:
  TransposedShiftMatrix(const Grammar &grammar);
};

// rows indexed by NTIndex, has variable length. Each elment in a row is a StatePair containing m_state,m_newState
// elements ordered by m_newState
class TransposedSuccessorMatrix : public StatePairMatrix {
public:
  TransposedSuccessorMatrix(const Grammar &grammar);
};
