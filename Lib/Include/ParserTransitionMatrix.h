#pragma once

#include "BitSet.h"
#include "MatrixTemplate.h"
#include "AbstractParserTables.h"

namespace LRParsing {

template<typename T> class ParserTransitionMatrix : public MatrixTemplate<T> {
protected:
  const AbstractSymbolNameContainer &m_nameContainer;
  const UINT m_symbolCount, m_termCount, m_ntermCount, m_stateCount;
  ParserTransitionMatrix(const AbstractParserTables &tables, const MatrixDimension &dim)
    : m_nameContainer(tables                 )
    , m_symbolCount(  tables.getSymbolCount())
    , m_termCount(    tables.getTermCount()  )
    , m_ntermCount(   tables.getNTermCount() )
    , m_stateCount(   tables.getStateCount() )
  {
    setDimension(dim);
  }

  BitSet getLegalColumns(UINT row) const {
    const size_t n = getColumnCount();
    BitSet result(n);
    for(UINT c = 0; c < n; c++) {
      const T v = (*this)(row, c);
      if(isValid(v)) {
        result.add(c);
      }
    }
    return result;
  }

  BitSet getLegalRows(UINT column) const {
    const size_t n = getRowCount();
    BitSet result(n);
    for(UINT r = 0; r < n; r++) {
      const T v = (*this)(r, column);
      if(isValid(v)) {
        result.add(r);
      }
    }
    return result;
  }
  virtual bool isValid(const T &v) const = 0;
};

inline String toString(const Action &a) {
  return a.toString();
}

class FullActionMatrix : public ParserTransitionMatrix<Action> {
public:
  FullActionMatrix(const AbstractParserTables &tables);
  String getLegalTermString(UINT state) const {
    return m_nameContainer.symbolSetToString(getLegalColumns(state));
  }
  String toString() const override;
  bool isValid(const Action &a) const final {
    return !a.isParserError();
  }
};

// SuccessorMatrix[state][NTindex] contains newstate or -1 if invalid combination
// state   = [0..m_stateCount-1]
// NTIndex = [0..m_NTCount-1   ]
class FullSuccessorMatrix : public ParserTransitionMatrix<SHORT> {
public:
  FullSuccessorMatrix(const AbstractParserTables &tables);
  String toString() const override;
  bool isValid(const SHORT &v) const final {
    return v >= 0;
  }
};

class FullTransitionMatrix : public ParserTransitionMatrix<Action> {
public:
  FullTransitionMatrix(const AbstractParserTables &tables);
  String getLegalSymbolString(UINT state) const {
    return m_nameContainer.symbolSetToString(getLegalColumns(state));
  }
  String toString() const override;
  bool isValid(const Action &a) const final {
    return !a.isParserError();
  }
};

class StatePair {
public:
  UINT m_state, m_newState;
  inline StatePair() : m_state(0), m_newState(0) {
  }
  inline StatePair(UINT state, UINT newState) : m_state(state), m_newState(newState) {
  }
};

static inline int stateSuccCmpByNewState(const StatePair &s1, const StatePair &s2) {
  return (int)s1.m_newState - (int)s2.m_newState;
}

class StatePairArray : public CompactArray<StatePair> {
private:
  const UINT m_stateCount;
public:
  inline StatePairArray(UINT stateCount) : m_stateCount(stateCount) {
  }
  inline void sortByNewState() {
    sort(stateSuccCmpByNewState);
  }
  String toString(bool forceAllStates) const;
};

class AbstractStatePairMatrix : public Array<StatePairArray> {
protected:
  const AbstractSymbolNameContainer &m_nameContainer;
  const UINT m_symbolCount, m_termCount, m_ntermCount, m_stateCount;
  AbstractStatePairMatrix(const AbstractParserTables &tables, UINT rowCount);
  // sort elements in each row by m_newState, incr.
  void sortAllRows();
  virtual String toString() const = 0;
};

// rows indexed by Term, has variable length. Each elment in a row is a StatePair containing m_state,m_newState, contains only shift-actions
class TransposedShiftMatrix :  public AbstractStatePairMatrix {
public:
  TransposedShiftMatrix(const AbstractParserTables &tables);
  String toString() const;
};

// rows indexed by NTIndex, has variable length. Each elment in a row is a StatePair containing m_state,m_newState
class TransposedSuccessorMatrix : public AbstractStatePairMatrix {
public:
  TransposedSuccessorMatrix(const AbstractParserTables &tables);
  String toString() const;
};

}; // namespace LRParsing
