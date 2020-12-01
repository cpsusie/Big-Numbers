#pragma once

#include "Grammar.h"
#include "IndexMap.h"
#include "MacroMap.h"
#include "StateActionNodeArray.h"

namespace ActionMatrixCompression {

class CompressedActionMatrix : public MacroMap {
private:
  const Grammar                &m_grammar;
  const GrammarResult          &m_grammarResult;
  const BitSetParam             m_usedByParam;
  const UINT                    m_sizeofTermBitSet;
  mutable TableTypeByteCountMap m_byteCountMap;
  UINT                          m_termArraySize;
  UINT                          m_actionArraySize;
  UINT                          m_splitNodeCount;
  StateActionNodeArray          m_stateActionNodeArray;
  TermSetIndexMap               m_termArrayMap;
  ActionArrayIndexMap           m_actionArrayMap;
  TermSetIndexMap               m_termBitSetMap;
  OptimizedBitSetPermutation    m_termBitSetPermutation;

  inline UINT getStateCount() const {
    return m_grammar.getStateCount();
  }
  Macro        doStateActionNode(    const StateActionNode &node);
  Macro        doBinSearchNode(      const StateActionNode &node);
  Macro        doSplitNode(          const StateActionNode &node);
  Macro        doImmediateNode(      const StateActionNode &node);
  Macro        doBitSetNode(         const StateActionNode &node);
  void         generateCompressedForm();
  void         printMacroesAndActionCodeArray(MarginFile &output) const;
  void         printTermAndActionArrayTable(  MarginFile &output) const;
  void         printTermBitSetTable(          MarginFile &output) const;

public:
  CompressedActionMatrix(const Grammar &grammar);
  inline const OptimizedBitSetPermutation &getTermBitSetPermutation() const {
    return m_termBitSetPermutation;
  }
  inline ByteCount getSavedBytesByOptimizedTermBitSets() const {
    return getTermBitSetPermutation().getSavedBytesByOptimizedBitSets((UINT)m_termBitSetMap.size());
  }
  const TableTypeByteCountMap &getByteCountMap() const {
    return m_byteCountMap;
  }

  ByteCount print(MarginFile &output) const;

  static TableTypeByteCountMap findTablesByteCount(const Grammar &grammar);
};

}; // namespace ActionMatrixCompression
