#include "stdafx.h"
#include "UIntPermutation.h"
#include "GrammarResult.h"

void GrammarResult::sortStateResult() {
  m_stateResult.sort(stateResultCmpByIndex);
}

void GrammarResult::reorderStates(const UIntPermutation &permutation) {
  permutation.validate();
  assert(permutation.size() == m_stateResult.size());
  for(auto it = m_stateResult.getIterator(); it.hasNext();) {
    StateResult &sr = it.next();
    sr.m_index = permutation[sr.m_index];
    for(auto it1 = sr.m_termActionArray.getIterator(); it1.hasNext();) {
      TermActionPair &tap = it1.next();
      if(tap.isShiftAction()) {
        tap.setNewState(permutation[tap.getNewState()]);
      }
    }
    for(auto it1 = sr.m_ntermNewStateArray.getIterator(); it1.hasNext();) {
      NTermNewStatePair &ntns = it1.next();
      ntns.setNewState(permutation[ntns.getNewState()]);
    }
  }
  sortStateResult();
}

void GrammarResult::addSRError(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_stateResult.last().m_errors.add(vformat(format, argptr));
  va_end(argptr);
  m_SRconflicts++;
}

void GrammarResult::addWarning(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  m_stateResult.last().m_warnings.add(vformat(format, argptr));
  va_end(argptr);
  m_warningCount++;
}

String GrammarResult::toString(const AbstractSymbolNameContainer &nameContainer) const {
  UINT state = 0;
  String result;
  for(auto it = m_stateResult.getIterator(); it.hasNext(); state++) {
    const StateResult &sr = it.next();
    assert(sr.m_index == state);
    String result1 = format(_T("State %4d\n"), state);
    result1 += format(_T("  Actions  :%s\n"), sr.m_termActionArray.toString(   nameContainer).cstr());
    result1 += format(_T("  NewStates:%s\n"), sr.m_ntermNewStateArray.toString(nameContainer).cstr());
    result += result1;
  }
  return result;
}
