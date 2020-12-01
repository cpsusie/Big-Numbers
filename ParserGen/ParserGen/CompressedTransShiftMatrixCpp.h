#pragma once

#include "GrammarTables.h"
#include "SymbolNodeArray.h"
#include "ReduceNodeArray.h"
#include "IndexMap.h"
#include "MacroMap.h"
#include "CompressedTransSuccMatrixCpp.h"

namespace TransposedShiftMatrixCompression {

using namespace TransposedSuccessorMatrixCompression;

class CompressedTransShiftMatrix {
private:
  const Grammar                      &m_grammar;
  const GrammarResult                &m_grammarResult;
  const UINT                          m_maxTermNameLength;
  const CompressedTransSuccMatrix     m_transSuccMatrix;

  MacroMap                            m_shiftMacroMap;
  const BitSetParam                   m_shiftUsedByParam;
  const BitSetInterval                m_shiftStateBitSetInterval;
  UINT                                m_fromStateArraySize , m_newStateArraySize, m_shiftSplitNodeCount;
  SymbolNodeArray                     m_shiftNodeArray;
  StateSetIndexMap                    m_fromStateArrayMap;
  StateArrayIndexMap                  m_newStateArrayMap;
  StateSetIndexMap                    m_stateBitSetMap;
  mutable OptimizedBitSetPermutation2 m_stateBitSetPermutation;
  mutable TableTypeByteCountMap       m_byteCountMap;

  MacroMap                            m_reduceMacroMap;
  const BitSetParam                   m_reduceUsedByParam;
  const UINT                          m_sizeofTermBitSet;
  UINT                                m_termArraySize , m_reduceArraySize, m_reduceSplitNodeCount;
  ReduceNodeArray                     m_reduceNodeArray;
  TermSetIndexMap                     m_termArrayMap;
  ActionArrayIndexMap                 m_reduceArrayMap;
  TermSetIndexMap                     m_termBitSetMap;
  mutable OptimizedBitSetPermutation  m_termBitSetPermutation;

  inline UINT         getStateCount() const {
    return m_grammar.getStateCount();
  }
  inline UINT         getTermCount() const {
    return m_grammar.getTermCount();
  }

  Macro               doShiftNode(          const SymbolNode &node);
  Macro               doShiftNodeBinSearch( const SymbolNode &node);
  Macro               doShiftNodeSplit(     const SymbolNode &node);
  Macro               doShiftNodeImmediate( const SymbolNode &node);
  Macro               doShiftNodeBitSet(    const SymbolNode &node);
  void                generateCompressedFormShift();
  void                findStateBitSetPermutation() const;
  void                findTermBitSetPermutation() const;

  Macro               doReduceNode(         const ReduceNode &node);
  Macro               doReduceNodeBinSearch(const ReduceNode &node);
  Macro               doReduceNodeSplit(    const ReduceNode &node);
  Macro               doReduceNodeImmediate(const ReduceNode &node);
  Macro               doReduceNodeBitSet(   const ReduceNode &node);
  void                generateCompressedFormReduce();

  void                generateCompressedForm();

  void                printMacroesAndShiftCodeArray(  MarginFile &output) const;
  void                printStatePairArrayTables(      MarginFile &output) const;
  void                printStateBitSetTable(          MarginFile &output) const;
  void                printShift(                     MarginFile &output) const;

  void                printMacroesAndReduceCodeArray( MarginFile &output) const;
  void                printTermAndReduceArrayTable(   MarginFile &output) const;
  void                printTermBitSetTable(           MarginFile &output) const;
  void                printReduce(                    MarginFile &output) const;

  void                printSucc(                      MarginFile &output) const;

public:
  CompressedTransShiftMatrix(const Grammar &grammar);

  const CompressedTransSuccMatrix &getTransSuccMatrix() const {
    return m_transSuccMatrix;
  }

  BitSet getStateBitSetTableUnion() const;
  BitSet getTermBitSetTableUnion()  const;

  const OptimizedBitSetPermutation2 &getStateBitSetPermutation() const;
  const OptimizedBitSetPermutation  &getTermBitSetPermutation()  const;

  ByteCount getSavedBytesByOptimizedStateBitSets() const;

  inline ByteCount getSavedBytesByOptimizedTermBitSets() const {
    return getTermBitSetPermutation().getSavedBytesByOptimizedBitSets(getTermBitSetCount());
  }
  inline UINT getStateBitSetCount() const {
    return (UINT)m_stateBitSetMap.size();
  }
  inline UINT getTermBitSetCount() const {
    return (UINT)m_termBitSetMap.size();
  }

  const TableTypeByteCountMap &getByteCountMap() const {
    return m_byteCountMap;
  }

  ByteCount print(MarginFile &output) const;

  static TableTypeByteCountMap findTablesByteCount(const Grammar &grammar);
};

}; // namespace TransposedShiftMatrixCompression
