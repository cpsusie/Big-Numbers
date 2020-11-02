#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

class SameReduceActionInfo {
private:
  const UINT m_prod;
  UINT       m_termSetSize; // == m_termSet.size()
  SymbolSet  m_termSet;     // set of terminals which should give reduce by procuction m_prod
public:
  SameReduceActionInfo(UINT terminalCount, UINT prod, UINT term0)
    : m_prod(       prod         )
    , m_termSetSize(0            )
    , m_termSet(    terminalCount)
  {
    addTerminal(term0);
  }
  inline UINT getProduction() const {
    return m_prod;
  }
  inline const SymbolSet &getTerminalSet() const {
    return m_termSet;
  }
  inline void addTerminal(UINT term) {
    m_termSet.add(term);
    m_termSetSize++;
  }
  inline UINT getSetSize() const {
    return m_termSetSize;
  }
  operator ActionArray() const;
};

inline int setSizeCmp(const SameReduceActionInfo &i1, const SameReduceActionInfo &i2) {
  return (int)i1.getSetSize() - (int)i2.getSetSize();
}

class SameReduceActionArray : public Array<SameReduceActionInfo> {
public:
  void sortBySetSize() {
    if(size() > 1) {
      sort(setSizeCmp);
    }
  }
};

class StateActionInfo {
private:
  const UINT             m_state, m_terminalCount, m_legalTokenCount;

  // List of different reduceActions;
  SameReduceActionArray  m_sameReductionArray;
  ActionArray            m_shiftActionArray;
  CompressionMethod      m_compressMethod;
  StateActionInfo &operator=(const StateActionInfo &src); // not implemented
  CompressionMethod findCompressionMethod();
public:
  StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray);

  // Return m_sameReductionArray.size() + m_shiftActionArray.size()
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
  inline CompressionMethod getCompressionMethod() const {
    return m_compressMethod;
  }

  String toString() const {
    return format(_T("%2u sa, %2u ra"), (UINT)m_shiftActionArray.size(), (UINT)m_sameReductionArray.size());
  }
};
