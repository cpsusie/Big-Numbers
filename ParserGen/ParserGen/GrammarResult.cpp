#include "stdafx.h"
#include "GrammarResult.h"

void GrammarResult::sortStateResult() {
  m_stateResult.sort(stateResultCmpByIndex);
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
