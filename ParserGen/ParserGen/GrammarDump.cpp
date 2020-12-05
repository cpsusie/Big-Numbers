#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"

void Grammar::dump(const SymbolSet &set, MarginFile *f) const {
  f->printf(_T("%s"), symbolSetToString(set).cstr());
}

void Grammar::dump(const Production &prod, MarginFile *f) const {
  fprintf(f, _T("%s prec %d"), prod.toString(*this).cstr(), prod.m_precedence);
}

void Grammar::dump(const LR1Item &item, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), itemToString(item, flags).cstr());
}

void Grammar::dump(const LR1State &state, int flags, MarginFile *f) const {
  f->printf(_T("%s\n"), stateToString(state, flags).cstr());
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
  const UINT maxLength = getMaxNTermNameLength() + 1;
  for(UINT s = getTermCount(); s < getSymbolCount(); s++) {
    const GrammarSymbol &nt = getSymbol(s);
    _ftprintf(f, _T("%-*.*s:%s\n"), maxLength, maxLength, nt.m_name.cstr(), symbolSetToString(nt.m_first1).cstr());
  }
}

String Grammar::itemToString(const LR1Item &item, int flags) const {
  String result;
  const Production &prod = getProduction(item.m_prod);
  result = format(_T(" (%3u)%c %-15s -> "), item.m_prod, item.m_kernelItem?'K':' ', getSymbol(prod.m_leftSide).m_name.cstr());
  for(UINT i = 0; i < item.m_dot; i++) {
    result += getSymbol(prod.m_rightSide[i]).m_name;
    result += ' ';
  }
  result += '.';
  const UINT n     = prod.getLength();
  TCHAR      delim = 0;
  for(UINT i = item.m_dot; i < n; i++, delim = ' ') {
    if(delim) result += delim;
    result += getSymbol(prod.m_rightSide[i]).m_name;
  }
  if(flags & DUMP_LOOKAHEAD) {
    result += symbolSetToString(item.m_la);
  }

  if((flags & DUMP_SUCC) && (item.m_succ >= 0)) {
    result += format(_T(" -> %d"), item.getSuccessor()); // ie not reduce-item
  }
  return result;
}

String Grammar::stateToString(const LR1State &state, int flags) const {
  String result;
  result = format(_T("State %u:\n"), state.m_index);
  const UINT itemstodump = (flags & DUMP_KERNELONLY) ? state.m_kernelItemCount : (UINT)state.m_items.size();
  for(UINT i = 0; i < itemstodump; i++) {
    const LR1Item &item = state.m_items[i];
    if(isShiftItem(item) && !(flags & DUMP_SHIFTITEMS)) {
      continue;
    }
    result += itemToString(state.m_items[i], flags) + _T("\n");
  }
  if(flags & DUMP_ACTIONS) {
    result += _T("\n");
    const StateResult            &sr                 = getResult().m_stateResult[state.m_index];
    const TermActionPairArray    &termActionArray    = sr.m_termActionArray;
    const NTermNewStatePairArray &ntermNewStateArray = sr.m_ntermNewStateArray;
    result += termActionArray.toString(   *this);
    result += ntermNewStateArray.toString(*this);
  }
  return result;
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

String Production::toString(const AbstractSymbolNameContainer &nameContainer) const {
  return nameContainer.getSymbolName(m_leftSide)
       + _T(" -> ")
       + getRightSideString(nameContainer);
}

String Production::getRightSideString(const AbstractSymbolNameContainer &nameContainer) const {
  const UINT n = getLength();

  if(n == 0) {
    return _T("epsilon");
  }

  String result;
  for(UINT i = 0; i < n; i++) {
    const UINT s = m_rightSide[i];
    if(i > 0) {
      result += ' ';
    }
    result += m_rightSide[i].toString(nameContainer);
  }
  return result;
}
