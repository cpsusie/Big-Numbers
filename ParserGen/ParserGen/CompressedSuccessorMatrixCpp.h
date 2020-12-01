#pragma once

#include "GrammarTables.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace SuccessorMatrixCompression {

inline int ntIndexSetCmp(const NTIndexSet &s1, const NTIndexSet &s2) {
  assert(s1.getCapacity() == s2.getCapacity());
  return bitSetCmp(s1, s2);
}

class NTIndexSetIndexMap : public IndexMap<NTIndexSet> {
public:
  NTIndexSetIndexMap() : IndexMap(ntIndexSetCmp) {
  }
};

typedef IndexArray<NTIndexSet> NTIndexSetIndexArray;

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
  UINT                          m_ntIndexArraySize;
  UINT                          m_newStateArraySize;
  NTIndexSetIndexMap            m_ntIndexArrayMap;
  StateArrayIndexMap            m_newStateArrayMap;
  StateSet                      m_definedStateSet;

  inline UINT getStateCount() const {
    return m_grammar.getStateCount();
  }
  Macro        doSuccessorArray(       UINT state, const SuccessorStateArray &successorArray);
  Macro        doNTIndexArrayState(    UINT state, const SuccessorStateArray &successorArray);
  Macro        doOneSuccessorState(    UINT state, const SuccessorState      &ss            );
  void         generateCompressedForm();

  void         printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  void         printNTIndexAndNewStateArray(     MarginFile &output) const;

public:
  CompressedSuccessorMatrix(const Grammar &grammar);
  ByteCount print(MarginFile &output) const;
  const TableTypeByteCountMap &getByteCountMap() const {
    return m_byteCountMap;
  }

  static TableTypeByteCountMap findTablesByteCount(const Grammar &grammar);
};

}; // namespace SuccessorMatrixCompression
