#include "pch.h"
#include "NFAState.h"

NFAState *NFAStatePool::fetch(int edge) {
  NFAState *s = __super::fetch();
  s->m_next = nullptr;
  assert(s->m_edge == EDGE_UNUSED);
  assert(!s->isMarked());
  s->setEdge(edge);
  return s;
}

void NFAStatePool::release(NFAState *s) {
  assert(s->m_edge != EDGE_UNUSED);
  assert(!s->isMarked());
  s->cleanup();
  s->m_edge  = EDGE_UNUSED;
  __super::release(s);
}

NFAState::NFAState() { // private
  m_id          = -1;
  m_startState  = false;
  m_marked      = false;
  m_edge        = EDGE_UNUSED;
  m_charClass   = nullptr;
  m_next        = m_next2 = nullptr;
  m_acceptIndex = -1;
}

NFAState &NFAState::operator=(const NFAState &src) {
  cleanup();
  copy(src);
  return *this;
}

NFAState::~NFAState() { // private
  cleanup();
}

void NFAState::copy(const NFAState &src) {
  assert(src.m_edge != EDGE_UNUSED);
  assert(!src.isMarked());
  m_id          = src.m_id;
  m_startState  = src.m_startState;
  m_edge        = src.m_edge;
  m_acceptIndex = src.m_acceptIndex;
  m_next        = src.m_next;
  m_next2       = src.m_next2;

  if(src.m_charClass) {
    assert(src.m_edge == EDGE_CHCLASS);
    m_charClass = new CharacterSet(*src.m_charClass); TRACE_NEW(m_charClass);
  } else {
    assert(src.m_edge != EDGE_CHCLASS);
    m_charClass = nullptr;
  }
}

void NFAState::cleanup() {
  assert(!isMarked());
  if(m_charClass) {
    assert(m_edge == EDGE_CHCLASS);
    SAFEDELETE(m_charClass);
  } else {
    assert(m_edge != EDGE_CHCLASS);
  }

  m_id          = -1;
  m_startState  = false;
  m_edge        = EDGE_EPSILON;
  m_next        = m_next2 = nullptr;
  m_acceptIndex = -1;
}

void NFAState::setEdge(int edge) {
  if(edge != m_edge) {
    if(m_edge == EDGE_CHCLASS) {
      SAFEDELETE(m_charClass);
    }
  }
  m_edge = edge;
  if(m_edge == EDGE_CHCLASS) {
    m_charClass = new CharacterSet; TRACE_NEW(m_charClass);
  }
}

NFAState *NFAState::getSuccessor(int c) const {
  return ((m_edge == c) || ((m_edge == EDGE_CHCLASS) && m_charClass->contains(c))) ? m_next : nullptr;
}

CharacterSet &NFAState::getCharacterSet() const {
  assert(m_edge == EDGE_CHCLASS);
  return *m_charClass;
}

int NFAState::compareById(NFAState * const &s1, NFAState * const &s2) { // static
  return s1->m_id - s2->m_id;
}
