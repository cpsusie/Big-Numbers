#pragma once

#include "GrammarTables.h"
#include "NTindexNodeArray.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace TransSuccMatrixCompression {

String getNTindexSetComment(const NTindexSet &NTindexSet);

inline int stateSetCmp(const StateSet &s1, const StateSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class StateSetIndexMap : public IndexMap<StateSet> {
public:
  StateSetIndexMap() : IndexMap(stateSetCmp) {
  }
};

typedef IndexArray<StateSet> StateSetIndexArray;

class StateArrayIndexMap : public IndexMap<StateArray> {
public:
  StateArrayIndexMap() : IndexMap(stateArrayCmp) {
  }
};

typedef IndexArray<StateArray> StateArrayIndexArray;

class CompressedTransSuccMatrix : public MacroMap {
private:
  const AbstractParserTables   &m_tables;
  const UINT                    m_stateSetSizeInBytes;
  UINT                          m_currentFromStateListSize;
  UINT                          m_currentNewStateListSize;
  UINT                          m_currentStateSetArraySize;
  UINT                          m_currentSplitNodeCount;
  const IntegerType             m_NTindexType, m_stateType;
  NTindexNodeArray              m_NTindexArray;
  StateSetIndexMap              m_stateSetMap;
  StateSetIndexMap              m_fromStateListMap;
  StateArrayIndexMap            m_newStateListMap;

  inline UINT getStateCount() const {
    return m_tables.getStateCount();
  }
  inline UINT getNTermCount() const {
    return m_tables.getNTermCount();
  }
  Macro        doNTindexNode(         const NTindexNode &node);
  Macro        doStatePairListNode(   const NTindexNode &node);
  Macro        doSplitNode(           const NTindexNode &node);
  Macro        doOneStatePairNode(    const NTindexNode &node);
  Macro        doStatePairSetNode(    const NTindexNode &node);
  void         generateCompressedForm();

  ByteCount    printMacroesAndSuccCode(MarginFile &output) const;
  ByteCount    printStatePairListTable(MarginFile &output) const;
  ByteCount    printStatePairSetTable( MarginFile &output) const;

public:
  CompressedTransSuccMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace TransSuccMatrixCompression
