#pragma once

#include "GrammarTables.h"
#include "CompressEncoding.h"

class SameReduceActionInfo {
private:
  const UINT                 m_prod;
  UINT                       m_termSetSize; // == m_termSet.size()
  SymbolSet                  m_termSet;     // set of terminals which should give reduce by procuction m_prod
  const SymbolNameContainer &m_symbolNames;
public:
  SameReduceActionInfo(UINT terminalCount, UINT prod, UINT term0, const SymbolNameContainer &symbolNames)
    : m_prod(       prod         )
    , m_symbolNames(symbolNames  )
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
  String toString() const {
    return format(_T("Reduce by %u on %s (%u terminals)"), m_prod, m_symbolNames.symbolSetToString(m_termSet).cstr(), m_termSetSize);
  }
};

inline int setSizeReverseCmp(const SameReduceActionInfo &i1, const SameReduceActionInfo &i2) {
  return (int)i2.getSetSize() - (int)i1.getSetSize();
}

class SameReduceActionArray : public Array<SameReduceActionInfo> {
public:
  // sort by setSize, decreasing, ie. largest set first
  inline void sortBySetSize() {
    if(size() > 1) {
      sort(setSizeReverseCmp);
    }
  }
  String toString() const {
    String result;
    for(auto it = getIterator(); it.hasNext();) {
      result += format(_T("   %s\n"), it.next().toString().cstr());
    }
    return result;
  }
};

class StateActionInfo {
private:
  const UINT                 m_state, m_terminalCount, m_legalTokenCount;
  const SymbolNameContainer &m_symbolNames;
  // List of different reduceActions;
  SameReduceActionArray      m_sameReductionArray;
  ActionArray                m_shiftActionArray;
  CompressionMethod          m_compressMethod;
  const StateActionInfo     *m_child[2]; // only used for m_compressMethod = SPLITNODECOMPRESSION
  StateActionInfo(           const StateActionInfo &src); // not implemented
  StateActionInfo &operator=(const StateActionInfo &src); // not implemented
  CompressionMethod findCompressionMethod();
  inline void initChildren() {
    m_child[0] = m_child[1] = nullptr;
  }
  StateActionInfo(const StateActionInfo &parent, const SameReduceActionInfo &sameReduceAction);
  StateActionInfo(const StateActionInfo &parent, const ActionArray          &shiftActionArray);
public:
  StateActionInfo(UINT terminalCount, UINT state, const ActionArray &actionArray, const SymbolNameContainer &symbolNames);
  ~StateActionInfo();
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
  const StateActionInfo &getChild(BYTE index) const {
    assert(getCompressionMethod() == ParserTables::CompCodeSplitNode);
    assert(index < 2);
    return *m_child[index];
  }

  String toString() const;
};
