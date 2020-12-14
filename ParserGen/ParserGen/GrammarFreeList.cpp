#include "stdafx.h"
#include "Grammar.h"

class LR1ItemList : public LR1Item {
public:
  LR1ItemList *m_next;
  LR1ItemList(LR1ItemList *next, const Grammar *grammar, bool kernelItem, UINT prod, UINT dot, const TermSet &la)
    : LR1Item(grammar, kernelItem, prod, dot, la)
    , m_next(next)
  {
  }
};

class LR1StateList : public LR1State {
public:
  LR1StateList *m_next;
  LR1StateList(LR1StateList *next, const Grammar *grammar, UINT index)
    : LR1State(grammar, index)
    , m_next(next)
  {
  }
};

void Grammar::initFreeLists() {
  m_itemList  = nullptr;
  m_stateList = nullptr;
}

LR1Item *Grammar::fetchItem(bool kernelItem, UINT prod, UINT dot, const TermSet &la) const {
  LR1ItemList *p;
  if(m_itemList == nullptr) {
    p = new LR1ItemList(nullptr, this, kernelItem, prod, dot, la); TRACE_NEW(p);
  } else {
    p               = m_itemList;
    m_itemList      = p->m_next;
    p->setValues(kernelItem, prod, dot, la);
  }
  return p;
}

LR1Item *Grammar::fetchItem(const LR1Item *src) const {
  LR1Item *copy = fetchItem(src->m_kernelItem, src->m_prod, src->m_dot, src->m_la);
  copy->m_newState = src->m_newState;
  return copy;
}

void Grammar::releaseItem(LR1Item *item) const {
  LR1ItemList *p = (LR1ItemList*)item;
  p->m_next      = m_itemList;
  m_itemList     = p;
}

void Grammar::clearItemList() const {
  while(m_itemList) {
    LR1ItemList *p = m_itemList->m_next;
    SAFEDELETE(m_itemList);
    m_itemList = p;
  }
}

LR1State *Grammar::fetchState() const {
  LR1StateList *p;
  if(m_stateList == nullptr) {
    p = new LR1StateList(nullptr, this, getStateCount()); TRACE_NEW(p);
  } else {
    p               = m_stateList;
    m_stateList     = p->m_next;
    p->setValues(getStateCount());
  }
  return p;
}

LR1State *Grammar::fetchState(const LR1State *src) const {
  LR1State *copy = fetchState();
  copy->m_index = src->m_index;
  for(auto itemp : src->m_items) {
    copy->addItem(fetchItem(itemp));
  }
  return copy;
}

void Grammar::releaseState(LR1State *state) const {
  for(auto itemp : state->m_items) {
    releaseItem(itemp);
  }
  state->m_items.clear(-1);
  LR1StateList *p = (LR1StateList*)state;
  p->m_next       = m_stateList;
  m_stateList     = p;
}

void Grammar::clearStateList() const {
  while(m_stateList) {
    LR1StateList *p = m_stateList->m_next;
    SAFEDELETE(m_stateList);
    m_stateList = p;
  }
}
