#pragma once

#include <AbstractParserTables.h>
#include <MatrixTemplate.h>

class FullActionMatrix : public MatrixTemplate<SHORT> {
private:
  const UINT m_symbolCount, m_termCount, m_NTermCount, m_stateCount;
public:
  FullActionMatrix(const AbstractParserTables &tables);
  String toString() const override;
};

// SuccessorMatrix[state][NTindex] contains newstate or _ParserError if invalid combination
// state   = [0..m_stateCount-1]
// NTIndex = [0..m_NTCount-1   ]
class FullSuccessorMatrix : public MatrixTemplate<USHORT> {
private:
  const UINT m_symbolCount, m_termCount, m_NTermCount, m_stateCount;
public:
  FullSuccessorMatrix(const AbstractParserTables &tables);
  String toString() const override;
};

class StateSucc {
public:
  UINT m_state, m_newState;
  inline StateSucc() : m_state(0), m_newState(0) {
  }
  inline StateSucc(UINT state, UINT newState) : m_state(state), m_newState(newState) {
  }
};

static inline int stateSuccCmpByNewState(const StateSucc &s1, const StateSucc &s2) {
  return (int)s1.m_newState - (int)s2.m_newState;
}

class StateSuccArray : public CompactArray<StateSucc> {
private:
  const UINT m_stateCount;
public:
  inline StateSuccArray(UINT stateCount) : m_stateCount(stateCount) {
  }
  inline void sortByNewState() {
    sort(stateSuccCmpByNewState);
  }
  String toString() const;
};

// rows indexed by NTindex, has variable length. Each elment in a row is a StateSucc containing m_state,m_newState
class TransposeSuccessorMatrix: public Array<StateSuccArray> {
private:
  const UINT m_symbolCount, m_termCount, m_NTermCount, m_stateCount;
public:
  TransposeSuccessorMatrix(const AbstractParserTables &tables);
  String toString() const;
};
