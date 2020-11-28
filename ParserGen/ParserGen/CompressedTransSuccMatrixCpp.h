#pragma once

#include "GrammarTables.h"
#include "NTIndexNodeArray.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace TransposedSuccessorMatrixCompression {

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
  mutable TableTypeByteCountMap m_byteCountMap;
  UINT                          m_fromStateArraySize;
  UINT                          m_newStateArraySize;
  UINT                          m_splitNodeCount;
  NTIndexNodeArray              m_ntIndexNodeArray;
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
  Macro               doNTIndexNode(   const NTIndexNode &node);
  Macro               doBinSearchNode( const NTIndexNode &node);
  Macro               doSplitNode(     const NTIndexNode &node);
  Macro               doImmediateNode( const NTIndexNode &node);
  Macro               doBitSetNode(    const NTIndexNode &node);
  void                generateCompressedForm();

  void         printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  void         printStatePairArrayTables(        MarginFile &output) const;
  void         printStateBitSetTable(            MarginFile &output) const;

public:
  CompressedTransSuccMatrix(const Grammar &grammar);
  const OptimizedBitSetPermutation &getStateBitSetPermutation() const {
    return m_stateBitSetPermutation;
  }
  inline ByteCount getSavedBytesByOptimizedStateBitSets() const {
    return getStateBitSetPermutation().getSavedBytesByOptimizedBitSets((UINT)m_stateBitSetMap.size());
  }
  ByteCount print(MarginFile &output) const;

  static TableTypeByteCountMap findTablesByteCount(const Grammar &grammar);
};

}; // namespace TransposedSuccessorMatrixCompression
