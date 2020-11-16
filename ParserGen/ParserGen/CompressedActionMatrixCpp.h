#pragma once

#include "GrammarTables.h"
#include "StateActionInfoArray.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace ActionMatrixCompression {

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

class ActionArrayIndexMap : public IndexMap<ActionArray> {
public:
  ActionArrayIndexMap() : IndexMap(actionArrayCmp) {
  }
};

typedef IndexArray<ActionArray> ActionArrayIndexArray;

class CompressedActionMatrix : public MacroMap {
private:
  const GrammarTables          &m_tables;
  const UINT                    m_stateCount;
  const UINT                    m_termSetSizeInBytes;
  UINT                          m_currentTermListSize;
  UINT                          m_currentActionListSize;
  UINT                          m_currentTermSetArraySize;
  UINT                          m_currentSplitNodeCount;
  const IntegerType             m_terminalType, m_actionType;
  StateActionInfoArray          m_stateInfoArray;
  TermSetIndexMap               m_termSetMap;
  TermSetIndexMap               m_termListMap;
  ActionArrayIndexMap           m_actionListMap;

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

}; // namespace ActionMatrixCompression
