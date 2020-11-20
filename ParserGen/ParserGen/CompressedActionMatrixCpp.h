#pragma once

#include "GrammarTables.h"
#include "StateActionNodeArray.h"
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
  const UINT                    m_termSetSizeInBytes;
  UINT                          m_currentTermListSize;
  UINT                          m_currentActionListSize;
  UINT                          m_currentTermSetArraySize;
  UINT                          m_currentSplitNodeCount;
  const IntegerType             m_termType, m_actionType;
  StateActionNodeArray          m_stateActionNodeArray;
  TermSetIndexMap               m_termSetMap;
  TermSetIndexMap               m_termListMap;
  ActionArrayIndexMap           m_actionListMap;

  inline UINT getStateCount() const {
    return m_tables.getStateCount();
  }
  Macro        doStateActionNode(     const StateActionNode &node);
  Macro        doTermListNode(        const StateActionNode &node);
  Macro        doSplitNode(           const StateActionNode &node);
  Macro        doOneItemNode(         const StateActionNode &node);
  Macro        doTermSetNode(         const StateActionNode &node);
  void         generateCompressedForm();

  ByteCount    printMacroesAndActionCode(MarginFile &output) const;
  ByteCount    printTermAndActionList(   MarginFile &output) const;
  ByteCount    printTermSetTable(        MarginFile &output) const;

public:
  CompressedActionMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrixCompression
