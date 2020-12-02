#include "stdafx.h"
#include "IndexMap.h"

static int symbolCmpByIndex(const GrammarSymbol &s1, const GrammarSymbol &s2) {
  return (int)s1.m_index - (int)s2.m_index;
}

static void reorderTermSet(TermSet &s, const CompactUIntArray &newOrder) {
  TermSet newSet(s.getCapacity());
  for(auto it = s.getIterator(); it.hasNext();) {
    newSet.add(newOrder[it.next()]);
  }
  s = newSet;
}

void Grammar::reorderTerminals(const OptimizedBitSetPermutation &permutation) {
  if(getTermReorderingDone()) {
    return;
  }
  if(permutation.size() != getTermCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("permutaton.size=%zu, termCount=%u"), permutation.size(), getTermCount());
  }
  if(!permutation.isEmpty() && permutation[0] != 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("permutaton[0] != 0. Must be 0 (=EOI)"));
  }
  if(permutation.getNewCapacity() > getTermCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("permutation.newCapacity=%u > termCount(=%u)"), permutation.getNewCapacity(), getTermCount());
  }
  permutation.validate();

  disableReorderTerminals();
  m_termBitSetCapacity  = permutation.getNewCapacity();

//  debugLog(_T("%s:m_termBitSetCapacity:%u\n"), __TFUNCTION__, m_termBitSetCapacity);

  const UINT symbolCount = getSymbolCount();
  const UINT termCount   = getTermCount();
  for(UINT oldIndex = 0; oldIndex < termCount; oldIndex++) {
    const UINT     newIndex = permutation[oldIndex];
    if(newIndex == oldIndex) {
      continue;
    }
    GrammarSymbol &gs       = m_symbolArray[oldIndex];
    UINT          *indexp   = m_symbolMap.get(gs.m_name.cstr());
    assert(newIndex < termCount);
    assert(indexp != nullptr   );
    gs.m_index = newIndex;
    *indexp    = newIndex;
  }
  m_symbolArray.sort(symbolCmpByIndex);
  for(auto it = m_productions.getIterator(); it.hasNext();) {
    Production                    &prod = it.next();
    CompactArray<RightSideSymbol> &rs   = prod.m_rightSide;
    const size_t                   len  = rs.size();
    for(UINT i = 0; i < len; i++) {
      RightSideSymbol &rss = rs[i];
      if(isTerminal(rss.m_index)) {
        rss.m_index = permutation[rss.m_index];
      }
    }
  }
  for(UINT nterm = termCount; nterm < symbolCount; nterm++) {
    GrammarSymbol &gs       = m_symbolArray[nterm];
    reorderTermSet(gs.m_first1, permutation);
  }
  for(auto it = m_states.getIterator(); it.hasNext();) {
    LR1State &state = it.next();
    for(auto it1 = state.m_items.getIterator(); it1.hasNext();) {
      LR1Item &item = it1.next();
      reorderTermSet(item.m_la, permutation);
    }
  }

  for(auto it = m_result.m_stateResult.getIterator(); it.hasNext();) {
    StateResult &sr = it.next();
    for(auto it1 = sr.m_actions.getIterator(); it1.hasNext();) {
      ParserAction &pa = it1.next();
      pa.m_term = permutation[pa.m_term];
    }
    sr.m_actions.sortByTerm();
  }
}

static int stateCmpByIndex(const LR1State &s1, const LR1State &s2) {
  return (int)s1.m_index - (int)s2.m_index;
}

void Grammar::reorderStates(const OptimizedBitSetPermutation2 &permutation) {
  if(getStateReorderingDone()) {
    return;
  }
  if(permutation.size() != getStateCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("permutaton.size=%zu, stateCount=%u"), permutation.size(), getStateCount());
  }
  permutation.validate();
  disableReorderStates();
  m_shiftStateSetInterval = permutation.getInterval(0);
  m_succStateSetInterval  = permutation.getInterval(1);

//  debugLog(_T("%s:shiftStateSetInterval:%s\n"), __TFUNCTION__, m_shiftStateSetInterval.toString().cstr());
//  debugLog(_T("%s:succStateSetInterval :%s\n"), __TFUNCTION__, m_succStateSetInterval.toString().cstr());

  for(auto it = m_states.getIterator(); it.hasNext();) {
    LR1State &state = it.next();
    state.m_index   = permutation[state.m_index];
    for(auto it1 = state.m_items.getIterator(); it1.hasNext();) {
      LR1Item &item = it1.next();
      if(item.m_succ >= 0) {
        item.m_succ = permutation[item.m_succ];
      }
    }
    UINT *indexp = m_stateMap.get(&state);
    *indexp = permutation[*indexp];
  }
  m_states.sort(stateCmpByIndex);
  m_startState = permutation[m_startState];
  for(auto it = m_result.m_stateResult.getIterator(); it.hasNext();) {
    StateResult &sr = it.next();
    sr.m_index = permutation[sr.m_index];
    for(auto it1 = sr.m_actions.getIterator(); it1.hasNext();) {
      ParserAction &pa = it1.next();
      if(pa.m_action > 0) {
        pa.m_action = permutation[pa.m_action];
      }
    }
    for(auto it1 = sr.m_succs.getIterator(); it1.hasNext();) {
      SuccessorState &ss = it1.next();
      ss.m_newState = permutation[ss.m_newState];
    }
  }
  m_result.sortStateResult();
}
