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
  const GrammarTables          &m_tables;
  const BitSetParam             m_usedByParam;
  const IntegerType             m_NTindexType, m_stateType;
  UINT                          m_NTindexArraySize;
  UINT                          m_newStateArraySize;
  NTindexSetIndexMap            m_NTindexArrayMap;
  StateArrayIndexMap            m_newStateArrayMap;
  StateSet                      m_definedStateSet;

  inline UINT getStateCount() const {
    return m_tables.getStateCount();
  }
  Macro        doSuccList(             UINT state, const SuccessorStateArray &succList);
  Macro        doNTindexListState(     UINT state, const SuccessorStateArray &succList);
  Macro        doOneSuccessorState(    UINT state, const SuccessorState      &ss      );
  void         generateCompressedForm();

  ByteCount    printMacroesAndSuccessorCodeArray(MarginFile &output) const;
  ByteCount    printNTindexAndNewStateArray(     MarginFile &output) const;

public:
  CompressedSuccessorMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace SuccessorMatrixCompression
