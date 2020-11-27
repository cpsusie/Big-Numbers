#pragma once

#include "GrammarTables.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace SuccessorMatrixCompression {

inline int NTindexSetCmp(const NTindexSet &s1, const NTindexSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class NTindexSetIndexMap : public IndexMap<NTindexSet> {
public:
  NTindexSetIndexMap() : IndexMap(NTindexSetCmp) {
  }
};

typedef IndexArray<NTindexSet> NTindexSetIndexArray;

class StateArrayIndexMap : public IndexMap<StateArray> {
public:
  StateArrayIndexMap() : IndexMap(stateArrayCmp) {
  }
};

typedef IndexArray<StateArray> StateArrayIndexArray;

class CompressedSuccessorMatrix : public MacroMap {
private:
  const Grammar                &m_grammar;
  const BitSetParam             m_usedByParam;
  mutable TableTypeByteCountMap m_byteCountMap;
  UINT                          m_NTindexArraySize;
  UINT                          m_newStateArraySize;
  NTindexSetIndexMap            m_NTindexArrayMap;
  StateArrayIndexMap            m_newStateArrayMap;
  StateSet                      m_definedStateSet;

  inline UINT getStateCount() const {
    return m_grammar.getStateCount();
  }
  Macro        doSuccessorArray(       UINT state, const SuccessorStateArray &successorArray);
  Macro        doNTindexArrayState(    UINT state, const SuccessorStateArray &successorArray);
  Macro        doOneSuccessorState(    UINT state, const SuccessorState      &ss            );
  void         generateCompressedForm();

  void         printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  void         printNTindexAndNewStateArray(     MarginFile &output) const;

public:
  CompressedSuccessorMatrix(const Grammar &grammar);
  ByteCount print(MarginFile &output) const;

  static TableTypeByteCountMap findTablesByteCount(const Grammar &grammar);
};

}; // namespace SuccessorMatrixCompression
