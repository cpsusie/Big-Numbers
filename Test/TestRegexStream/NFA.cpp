#include "stdafx.h"
#include "NFA.h"

NFA::NFA(NFAStatePool &statePool, NFAState *start) : m_statePool(statePool) {
  create(start);
}

void NFA::create(NFAState *start) {
  clear();
  m_idCounter = 0;
  addIfNotMarked(start);
  unmarkAll(start);
  sort(NFAState::compareById);
}

void NFA::addIfNotMarked(NFAState *s) {
  if(s && !s->isMarked()) {
    s->m_id = m_idCounter++;
    s->setMark(true);
    add(s);
    addIfNotMarked(s->m_next );
    addIfNotMarked(s->m_next2);
  }
}

void NFA::unmarkAll(NFAState *s) { // static
  if(s && s->isMarked()) {
    s->setMark(false);
    unmarkAll(s->m_next );
    unmarkAll(s->m_next2);
  }
}

NFA::~NFA() {
  for(size_t i = 0; i < size(); i++) {
    m_statePool.release((*this)[i]);
  }
  clear();
}

