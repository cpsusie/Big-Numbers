#pragma once

#include "GrammarTables.h"
#include "StateActionInfoArray.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace ActionMatrix {

inline int termSetCmp(const TermSet &s1, const TermSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class TermSetIndexMap : public IndexMap<TermSet> {
public:
  TermSetIndexMap() : IndexMap(termSetCmp) {
  }
};

typedef IndexArray<TermSet> TermSetIndexArray;

class RawActionArrayIndexMap : public IndexMap<RawActionArray> {
public:
  RawActionArrayIndexMap() : IndexMap<RawActionArray>(rawActionArrayCmp) {
  }
};

typedef IndexArray<RawActionArray> RawActionArrayIndexArray;

class CompressedActionMatrix : public MacroMap {
private:
  const GrammarTables          &m_tables;
  const UINT                    m_terminalCount;
  const UINT                    m_stateCount;
  const UINT                    m_laSetSizeInBytes;
  UINT                          m_currentTermListSize;
  UINT                          m_currentActionListSize;
  UINT                          m_currentLASetArraySize;
  UINT                          m_currentSplitNodeCount;
  const IntegerType             m_terminalType, m_actionType;
  StateActionInfoArray          m_stateInfoArray;
  TermSetIndexMap               m_laSetMap;
  TermSetIndexMap               m_termListMap;
  RawActionArrayIndexMap        m_raaMap;

  inline UINT getTerminalCount() const {
    return m_terminalCount;
  }
  inline UINT getStateCount() const {
    return m_stateCount;
  }
  Macro        doStateActionInfo(      const StateActionInfo &stateInfo);
  Macro        doTermListState(        const StateActionInfo &stateInfo);
  Macro        doSplitNodeState(       const StateActionInfo &stateInfo);
  Macro        doOneItemState(         const StateActionInfo &stateInfo);
  Macro        doTermSetState(         const StateActionInfo &stateInfo);
  void         generateCompressedForm();

  ByteCount    printMacroesAndActionCode(MarginFile &output) const;
  ByteCount    printTermAndActionList(   MarginFile &output) const;
  ByteCount    printTermSetTable(        MarginFile &output) const;

public:
  CompressedActionMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrix
