#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

class SameReduceActionInfo {
private:
  const UINT m_prod;
  SymbolSet  m_termSet;
public:
  SameReduceActionInfo(UINT terminalCount, UINT prod, UINT token)
    : m_prod(prod)
    , m_termSet(terminalCount)
  {
    m_termSet.add(token);
  }
  inline UINT getProduction() const {
    return m_prod;
  }
  inline const SymbolSet &getTerminalSet() const {
    return m_termSet;
  }
  inline void addTerminal(UINT token) {
    m_termSet.add(token);
  }
  operator ActionArray() const;
};

class SameReduceActionArray : public Array<SameReduceActionInfo> {
};

class StateActionInfo {
private:
  const UINT             m_state, m_terminalCount;
  // List of different reduceActions;
  SameReduceActionArray  m_sameReductionArray;
  ActionArray            m_shiftActionArray;
  StateActionInfo &operator=(const StateActionInfo &src); // not implemented
public:
  StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray);

  inline UINT getDifferentActionCount() const {
    return (UINT)(m_sameReductionArray.size() + m_shiftActionArray.size());
  }
  inline UINT getState() const {
    return m_state;
  }
  ActionArray getActionArray() const;
  inline const SameReduceActionArray &getReduceActionArray() const {
    return m_sameReductionArray;
  }
  inline const ActionArray &getShiftActionArray() const {
    return m_shiftActionArray;
  }
  CompressionMethod getCompressionMethod() const;

  String toString() const {
    return format(_T("%2u sa, %2u ra"), (UINT)m_shiftActionArray.size(), (UINT)m_sameReductionArray.size());
  }
};
