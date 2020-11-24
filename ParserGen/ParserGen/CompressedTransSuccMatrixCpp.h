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
  const Grammar                &m_grammar;
  const GrammarResult          &m_grammarResult;
  const BitSetParam             m_usedByParam;
  const UINT                    m_sizeofStateBitSet;
  const UINT                    m_maxNTermNameLength;
  UINT                          m_fromStateArraySize;
  UINT                          m_newStateArraySize;
  UINT                          m_splitNodeCount;
  NTindexNodeArray              m_NTindexNodeArray;
  StateSetIndexMap              m_fromStateArrayMap;
  StateArrayIndexMap            m_newStateArrayMap;
  StateSetIndexMap              m_stateBitSetMap;
  OptimizedBitSetPermutation    m_stateBitSetPermutation;

  inline UINT         getStateCount() const {
    return m_grammar.getStateCount();
  }
  inline UINT         getNTermCount() const {
    return m_grammar.getNTermCount();
  }
  Macro               doNTindexNode(   const NTindexNode &node);
  Macro               doBinSearchNode( const NTindexNode &node);
  Macro               doSplitNode(     const NTindexNode &node);
  Macro               doImmediateNode( const NTindexNode &node);
  Macro               doBitSetNode(    const NTindexNode &node);
  void                generateCompressedForm();

  ByteCount    printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  ByteCount    printStatePairArrayTables(        MarginFile &output) const;
  ByteCount    printStateBitSetTable(            MarginFile &output) const;

public:
  CompressedTransSuccMatrix(const Grammar &grammar);
  const OptimizedBitSetPermutation &getStateBitSetPermutation() const {
    return m_stateBitSetPermutation;
  }
  inline ByteCount getSavedBytesByOptimizedStateBitSets() const {
    return getStateBitSetPermutation().getSavedBytesByOptimizedBitSets((UINT)m_stateBitSetMap.size());
  }
  ByteCount print(MarginFile &output) const;
};

}; // namespace TransSuccMatrixCompression
