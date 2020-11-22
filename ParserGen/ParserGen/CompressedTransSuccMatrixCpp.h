#pragma once

#include "GrammarTables.h"
#include "NTindexNodeArray.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace TransSuccMatrixCompression {

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
  const BitSetParam             m_usedByParam;
  const UINT                    m_stateSetSizeInBytes;
  const UINT                    m_maxNTermNameLength;
  const IntegerType             m_NTindexType, m_stateType;
  UINT                          m_fromStateArraySize;
  UINT                          m_newStateArraySize;
  UINT                          m_splitNodeCount;
  NTindexNodeArray              m_NTindexNodeArray;
  StateSetIndexMap              m_fromStateArrayMap;
  StateArrayIndexMap            m_newStateArrayMap;
  StateSetIndexMap              m_stateBitSetMap;

  inline UINT getStateCount() const {
    return m_tables.getStateCount();
  }
  inline UINT getNTermCount() const {
    return m_tables.getNTermCount();
  }
  Macro        doNTindexNode(   const NTindexNode &node);
  Macro        doBinSearchNode( const NTindexNode &node);
  Macro        doSplitNode(     const NTindexNode &node);
  Macro        doImmediateNode( const NTindexNode &node);
  Macro        doBitSetNode(    const NTindexNode &node);
  void         generateCompressedForm();

  ByteCount    printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  ByteCount    printStatePairArrayTables(        MarginFile &output) const;
  ByteCount    printStateBitSetTable(            MarginFile &output) const;

public:
  CompressedTransSuccMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace TransSuccMatrixCompression
