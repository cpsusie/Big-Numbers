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
  const UINT                    m_stateCount;
  UINT                          m_currentNTindexListSize;
  UINT                          m_currentStateListSize;
  const IntegerType             m_NTindexType, m_stateType;
  NTindexSetIndexMap            m_NTindexMap;
  StateArrayIndexMap            m_stateListMap;
  StateSet                      m_definedStateSet;

  inline UINT getStateCount() const {
    return m_stateCount;
  }
  Macro        doSuccList(             UINT state, const SuccessorStateArray &succList);
  Macro        doNTindexListState(     UINT state, const SuccessorStateArray &succList);
  Macro        doOneSuccessorState(    UINT state, const SuccessorState      &ss      );
  void         generateCompressedForm();

  ByteCount    printMacroesAndSuccessorCode(MarginFile &output) const;
  ByteCount    printNTindexAndStateList(    MarginFile &output) const;

public:
  CompressedSuccessorMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace SuccessorMatrixCompression
