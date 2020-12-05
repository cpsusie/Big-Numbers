#pragma once

#include "GrammarTables.h"
#include "SymbolNodeArray.h"
#include "IndexMap.h"
#include "MacroMap.h"
#include "TableTypeByteCountMap.h"
#include "OptimizedBitSetPermutation.h"

class CompressedTransSuccMatrix : public MacroMap {
private:
  const Grammar                     &m_grammar;
  const GrammarResult               &m_grammarResult;
  const BitSetParameters             m_usedByParam;
  const BitSetInterval               m_bitSetInterval;
  const UINT                         m_maxNTermNameLength;
  UINT                               m_fromStateArraySize;
  UINT                               m_newStateArraySize;
  UINT                               m_splitNodeCount;
  SymbolNodeArray                    m_ntIndexNodeArray;
  StateSetIndexMap                   m_fromStateArrayMap;
  StateArrayIndexMap                 m_newStateArrayMap;
  StateSetIndexMap                   m_stateBitSetMap;
  mutable OptimizedBitSetPermutation m_stateBitSetPermutation;
  mutable TableTypeByteCountMap      m_byteCountMap;

  inline UINT         getStateCount() const {
    return m_grammar.getStateCount();
  }
  inline UINT         getNTermCount() const {
    return m_grammar.getNTermCount();
  }
  Macro               doNTIndexNode(   const SymbolNode &node);
  Macro               doBinSearchNode( const SymbolNode &node);
  Macro               doSplitNode(     const SymbolNode &node);
  Macro               doImmediateNode( const SymbolNode &node);
  Macro               doBitSetNode(    const SymbolNode &node);
  void                generateCompressedForm();
  void                findStateBitSetPermutation() const;

  void         printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  void         printStatePairArrayTables(        MarginFile &output) const;
  void         printStateBitSetTable(            MarginFile &output) const;

public:
  CompressedTransSuccMatrix(const Grammar &grammar);

  BitSet getBitSetsTableUnion() const;
  const OptimizedBitSetPermutation &getStateBitSetPermutation() const;
  inline ByteCount getSavedBytesByOptimizedStateBitSets() const {
    return getStateBitSetPermutation().getSavedBytesByOptimizedBitSets(getStateBitSetCount());
  }

  inline UINT getStateBitSetCount() const {
    return (UINT)m_stateBitSetMap.size();
  }
  const TableTypeByteCountMap &getByteCountMap() const {
    return m_byteCountMap;
  }
  
  ByteCount print(MarginFile &output) const;
};

