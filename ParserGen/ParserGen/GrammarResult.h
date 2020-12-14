#pragma once

#include "StateResult.h"

class UIntPermutation;

class GrammarResult {
public:
  Array<StateResult> m_stateResult;
  UINT               m_SRconflicts, m_RRconflicts;         // only for doc-file
  UINT               m_warningCount;
  GrammarResult()
    : m_SRconflicts(0)
    , m_RRconflicts(0)
    , m_warningCount(0)
  {
  }
  GrammarResult &clear(UINT capacity) {
    m_SRconflicts = m_RRconflicts = 0; // don't erase warnings
    m_stateResult.clear(capacity);
    return *this;
  }
  inline bool    allStatesConsistent() const {
    return (m_SRconflicts == 0) && (m_RRconflicts == 0);
  }
  inline UINT    getStateCount() const {
    return (UINT)m_stateResult.size();
  }
  void           sortStateResult();
  void           reorderStates(const UIntPermutation &permutation);
  void           addSRError(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void           addWarning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  String         toString(const AbstractSymbolNameContainer &nameContainer) const;
};

