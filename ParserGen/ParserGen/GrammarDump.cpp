#include "stdafx.h"

void Grammar::dump(const SymbolSet &set, MarginFile *f) const {
  f->printf(_T("%s"), symbolSetToString(set).cstr());
}

void Grammar::dump(const Production &prod, MarginFile *f) const {
  fprintf(f, _T("%s -> "), getSymbol(prod.m_leftSide).m_name.cstr());
  for(UINT i = 0; i < prod.getLength(); i++) {
    fprintf(f, _T("%s "), getSymbol(prod.m_rightSide[i]).m_name.cstr());
  }
  fprintf(f, _T("prec %d"), prod.m_precedence);
}

void Grammar::dump(const LR1Item &item, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), itemToString(item, flags).cstr());
}

void Grammar::dump(const LR1State &state, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), stateToString(state, flags).cstr());
  if(flags & DUMP_ERRORS) {
    const StringArray &errors = m_result[state.m_index].m_errors;
    for(size_t i = 0; i < errors.size(); i++) {
      _tprintf(_T("%s(%d) : error --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), errors[i].cstr());
      f->printf(_T("error --- %s\n"), errors[i].cstr());
    }
  }
  if(flags & DUMP_WARNINGS) {
    const StringArray &warnings = m_result[state.m_index].m_warnings;
    for(size_t i = 0; i < warnings.size(); i++) {
      verbose(2, _T("%s(%d) : warning --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), warnings[i].cstr());
      f->printf(_T("warning --- %s\n"), warnings[i].cstr());
    }
  }
}

void Grammar::dumpStates(int flags, MarginFile *f) const {
  for(UINT i = 0; i < getStateCount(); i++) {
    dump(m_states[i], flags, f);
  }
}

void Grammar::dump(MarginFile *f) const {
  for(UINT s = 0; s < getSymbolCount(); s++) {
    const GrammarSymbol &sym = getSymbol(s);
    f->printf(_T("Symbol:%-20s, %4d %-11s "), sym.m_name.cstr(), sym.m_precedence, sym.getTypeString());
    if(sym.m_reachable    ) f->printf(_T("reachable "));
    if(sym.m_terminate    ) f->printf(_T("terminate "));
    if(sym.m_deriveEpsilon) f->printf(_T("derive e "));
    if(isNonTerminal(s)) {
      dump(sym.m_first1);
    }
    f->printf(_T("\n"));
  }

  for(UINT p = 0; p < getProductionCount(); p++) {
    dump(getProduction(p), f);
    f->printf(_T("\n"));
  }
}

void Grammar::dumpFirst1Sets(FILE *f) const {
  const UINT maxLength = getMaxNonTerminalNameLength() + 1;
  for(UINT s = getTerminalCount(); s < getSymbolCount(); s++) {
    const GrammarSymbol &nt = getSymbol(s);
    _ftprintf(f, _T("%-*.*s:%s\n"), maxLength, maxLength, nt.m_name.cstr(), symbolSetToString(nt.m_first1).cstr());
  }
}

