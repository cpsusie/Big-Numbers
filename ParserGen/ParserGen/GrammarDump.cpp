#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"

void Grammar::dump(const SymbolSet &set  , MarginFile *f) const {
  f->printf(_T("%s"), symbolSetToString(set).cstr());
}

void Grammar::dump(const Production &prod, MarginFile *f) const {
  fprintf(f, _T("%s prec %d"), prod.toString().cstr(), prod.m_precedence);
}

void Grammar::dump(const LR1Item &item   , UINT flags, MarginFile *f) const {
  f->printf(_T("%s\n"), item.toString(flags).cstr());
}

void Grammar::dump(const LR1State &state , UINT flags, MarginFile *f) const {
  f->printf(_T("%s\n"), state.toString(flags).cstr());
  const StateResult &sr = getResult().m_stateResult[state.m_index];
  if(flags & DUMP_ERRORS) {
    const StringArray &errors = sr.m_errors;
    for(size_t i = 0; i < errors.size(); i++) {
      _tprintf(_T("%s(%d) : error --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), errors[i].cstr());
      f->printf(_T("error --- %s\n"), errors[i].cstr());
    }
  }
  if(flags & DUMP_WARNINGS) {
    const StringArray &warnings = sr.m_warnings;
    for(size_t i = 0; i < warnings.size(); i++) {
      verbose(2, _T("%s(%d) : warning --- %s\n"), f->getAbsolutName().cstr(), f->getLineNumber(), warnings[i].cstr());
      f->printf(_T("warning --- %s\n"), warnings[i].cstr());
    }
  }
}

void Grammar::dumpStates(UINT flags, MarginFile *f) const {
  for(auto statep : m_stateArray) {
    dump(*statep, flags, f);
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
  const UINT maxLength = getMaxNTermNameLength() + 1;
  for(UINT s = getTermCount(); s < getSymbolCount(); s++) {
    const GrammarSymbol &nt = getSymbol(s);
    _ftprintf(f, _T("%-*.*s:%s\n"), maxLength, maxLength, nt.m_name.cstr(), symbolSetToString(nt.m_first1).cstr());
  }
}

BitSetParameters Grammar::getBitSetParam(ElementType type) const {
  switch(type) {
  case ETYPE_SYMBOL    : return BitSetParameters(type, getSymbolCount()    );
  case ETYPE_TERM      : return BitSetParameters(type, getTermCount()      );
  case ETYPE_NTINDEX   : return BitSetParameters(type, getNTermCount()     );
  case ETYPE_PRODUCTION: return BitSetParameters(type, getProductionCount());
  case ETYPE_STATE     : return BitSetParameters(type, getStateCount()     );
  default              : throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
  }
  return BitSetParameters(type, 0); // should never come here
}
