#pragma once

#include "GrammarTables.h"
#include "IndexMap.h"
#include "MacroMap.h"

namespace SuccessorMatrix {

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

class SuccessorArrayIndexMap : public IndexMap<SuccessorArray> {
public:
  SuccessorArrayIndexMap() : IndexMap<SuccessorArray>(successorArrayCmp) {
  }
};

typedef IndexArray<SuccessorArray> SuccessorArrayIndexArray;

class CompressedSuccessorMatrix : public MacroMap {
private:
  const GrammarTables          &m_tables;
  const UINT                    m_terminalCount;
  const UINT                    m_symbolCount;
  const UINT                    m_stateCount;
  UINT                          m_currentNTindexListSize;
  UINT                          m_currentStateListSize;
  const IntegerType             m_NTindexType, m_stateType;
  NTindexSetIndexMap            m_NTindexMap;
  SuccessorArrayIndexMap        m_successorMap;
  StateSet                      m_definedStateSet;

  inline UINT getTerminalCount() const {
    return m_terminalCount;
  }
  inline UINT getStateCount() const {
    return m_stateCount;
  }
  Macro        doSuccList(             UINT state, const ActionArray  &succList);
  Macro        doNTindexListState(     UINT state, const ActionArray  &succList);
  Macro        doOneSuccessorState(    UINT state, const ParserAction &pa      );
  void         generateCompressedForm();

  ByteCount    printMacroesAndSuccessorCode(MarginFile &output) const;
  ByteCount    printNTindexAndStateList(    MarginFile &output) const;

public:
  CompressedSuccessorMatrix(const GrammarTables &tables);
  ByteCount print(MarginFile &output) const;
};

}; // namespace SuccessorMatrix
