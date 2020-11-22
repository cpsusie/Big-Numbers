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
  const BitSetParam             m_usedByParam;
  const UINT                    m_termSetSizeInBytes;
  const IntegerType             m_termType, m_actionType;
  UINT                          m_termArraySize;
  UINT                          m_actionArraySize;
  UINT                          m_splitNodeCount;
  StateActionNodeArray          m_stateActionNodeArray;
  TermSetIndexMap               m_termArrayMap;
  ActionArrayIndexMap           m_actionArrayMap;
  TermSetIndexMap               m_termBitSetMap;

  inline UINT getStateCount() const {
    return m_tables.getStateCount();
  }
  Macro        doStateActionNode(    const StateActionNode &node);
  Macro        doBinSearchNode(      const StateActionNode &node);
  Macro        doSplitNode(          const StateActionNode &node);
  Macro        doImmediateNode(      const StateActionNode &node);
  Macro        doBitSetNode(         const StateActionNode &node);
  void         generateCompressedForm();

  ByteCount    printMacroesAndActionCodeArray(MarginFile &output) const;
  ByteCount    printTermAndActionArrayTable(  MarginFile &output) const;
  ByteCount    printTermBitSetTable(          MarginFile &output) const;

public:
  CompressedActionMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace ActionMatrixCompression
