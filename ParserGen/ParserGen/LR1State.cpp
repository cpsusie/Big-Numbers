#include "stdafx.h"
#include "Grammar.h"
#include "GrammarResult.h"

LR1Item *LR1State::findItemWithSameCore(const LR1Item *item) const {
  for(auto itemp : m_items) {
    if(itemp->hasSameCore(item)) {
      return itemp;
    }
  }
  return nullptr;
}

bool LR1State::hasSameCore(const LR1State &state) const {
  const BYTE n = m_kernelItemCount;
  if(n != state.m_kernelItemCount) {
    return false;
  }
  for(LR1Item *const *item1 = m_items.begin(), *const *endp = item1 + n, *const *item2 = state.m_items.begin(); item1 < endp; item2++) {
    if(!(*(item1++))->hasSameCore(*item2)) {
      return false;
    }
  }
  return true;
}

ULONG LR1State::hashCode() const {
  const BYTE n = m_kernelItemCount;
  ULONG      v = n;
  for(LR1Item *const *itemp = m_items.begin(), *const *endp = itemp + n; itemp < endp;) {
    v ^= (*(itemp++))->hashCode();
    v *= 13;
  }
  return v;
}

bool LR1State::mergeLookahead(const LR1State *src) {
  bool changed = false;
  LR1Item * const *srcItem = src->m_items.begin(), * const *endp = srcItem + src->m_kernelItemCount;
  for(LR1Item **dstItem = m_items.begin(); srcItem < endp; srcItem++, dstItem++) {
    assert((*dstItem)->hasSameCore(*srcItem));
    if(!((*srcItem)->m_la - (*dstItem)->m_la).isEmpty()) {
      (*dstItem)->m_la += (*srcItem)->m_la;
      changed  = true;
    }
  }
  return changed;
}

class ItemComparator : public Comparator<LR1Item *> {
public:
  int compare(LR1Item *const &e1, LR1Item *const &e2);
  AbstractComparator *clone() const {
    return new ItemComparator;
  }
  static ItemComparator s_compare;
};

int ItemComparator::compare(LR1Item *const &e1, LR1Item *const &e2) {
  int c = e2->m_kernelItem - e1->m_kernelItem;         // Used by sortItems. put kernelitems first
  if(c) {
    return c;
  }
  c = (int)(e1->m_prod) - (int)(e2->m_prod);           // Then sort by production
  return c ? c : (int)(e1->m_dot) - (int)(e2->m_dot);  // and by dot
}

ItemComparator ItemComparator::s_compare;

void LR1State::sortItems() {
  m_items.sort(ItemComparator::s_compare);
}

String LR1State::toString(UINT flags) const {
  String result;
  result = format(_T("State %u:\n"), m_index);
  const UINT itemsToDump = (flags & DUMP_KERNELONLY) ? m_kernelItemCount : (UINT)m_items.size();
  for(UINT i = 0; i < itemsToDump; i++) {
    const LR1Item &item = *m_items[i];
    if(item.isShiftItem() && !(flags & DUMP_SHIFTITEMS)) {
      continue;
    }
    result += item.toString(flags) + _T("\n");
  }
  if(flags & DUMP_ACTIONS) {
    result += _T("\n");
    const StateResult            &sr                 = m_grammar.getResult().m_stateResult[m_index];
    const TermActionPairArray    &termActionArray    = sr.m_termActionArray;
    const NTermNewStatePairArray &ntermNewStateArray = sr.m_ntermNewStateArray;
    result += termActionArray.toString(   m_grammar);
    result += ntermNewStateArray.toString(m_grammar);
  }
  return result;
}
