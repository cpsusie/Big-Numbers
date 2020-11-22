#include "stdafx.h"
#include "NFA.h"

#if defined(_DEBUG)

String AcceptType::toString() const {
  if(!m_acceptAttribute) {
    return _T("NonAccepting");
  } else {
    String result = _T("Accepting");
    if(m_acceptAttribute & ANCHOR_BOTH) {
      result += format(_T(" Anchor: %s%s")
                      ,m_acceptAttribute & ANCHOR_START ? _T("start ") : EMPTYSTRING
                      ,m_acceptAttribute & ANCHOR_END   ? _T("end")    : EMPTYSTRING);
    }
    return result;
  }
}

int                     NFAState::s_stateCount = 0;
CharacterFormater      *NFAState::s_formater   = &CharacterFormater::hexEscapedExtendedAsciiFormater;
CompactArray<NFAState*> NFAState::s_allocatedStates;

#define incrCount() s_stateCount++
#define decrCount() s_stateCount--

#else

#define incrCount()
#define decrCount()

#endif


HeapObjectPool<NFAState> NFAState::s_stateManager;

NFAState *NFAState::fetch(int edge) {  // static
  NFAState *s = s_stateManager.fetch();
  s->m_next = nullptr;
  assert(s->m_edge == EDGE_UNUSED);
  assert(!s->isMarked());
  s->setEdge(edge);
#if defined(_DEBUG)
  s->m_patternCharIndex = -1;
  s_allocatedStates.add(s);
#endif
  return s;
}

void NFAState::release(NFAState *s) {  // static
  assert(s->m_edge != EDGE_UNUSED);
  assert(!s->isMarked());
  s->cleanup();
  s->m_edge  = EDGE_UNUSED;
  s_stateManager.release(s);
}

void NFAState::releaseAll() {          // static
#if defined(_DEBUG)
  s_allocatedStates.clear();
#endif
  s_stateManager.releaseAll();
}

NFAState::NFAState() { // private
  m_id         = -1;
  m_startState = false;
  m_marked     = false;
  m_edge       = EDGE_UNUSED;
  m_charClass  = nullptr;
  m_next       = m_next2 = nullptr;
  incrCount();
}

NFAState &NFAState::operator=(const NFAState &src) {
  cleanup();
  copy(src);
  return *this;
}

NFAState::~NFAState() { // private
  cleanup();
  decrCount();
}

void NFAState::copy(const NFAState &src) {
  assert(src.m_edge != EDGE_UNUSED);
  assert(!src.isMarked());
  m_id         = src.m_id;
  m_startState = src.m_startState;
  m_edge       = src.m_edge;
  m_accept     = src.m_accept;
  m_next       = src.m_next;
  m_next2      = src.m_next2;

  if(src.m_charClass) {
    assert(src.m_edge == EDGE_CHCLASS);
    m_charClass = new CharacterSet(*src.m_charClass); TRACE_NEW(m_charClass);
  } else {
    assert(src.m_edge != EDGE_CHCLASS);
    m_charClass = nullptr;
  }
#if defined(_DEBUG)
  m_patternCharIndex = src.m_patternCharIndex;
#endif
}

void NFAState::cleanup() {
  assert(!isMarked());
  if(m_charClass) {
    assert(m_edge == EDGE_CHCLASS);
    SAFEDELETE(m_charClass);
    m_charClass = nullptr;
  } else {
    assert(m_edge != EDGE_CHCLASS);
  }

  m_id                       = -1;
  m_startState               = false;
  m_edge                     = EDGE_EPSILON;
  m_next                     = m_next2 = nullptr;
  m_accept.m_acceptAttribute = 0;
}

void NFAState::setEdge(int edge) {
  if(edge != m_edge) {
    if(m_edge == EDGE_CHCLASS) {
      SAFEDELETE(m_charClass);
      m_charClass = nullptr;
    }
  }
  m_edge = edge;
  if(m_edge == EDGE_CHCLASS) {
    m_charClass = new CharacterSet; TRACE_NEW(m_charClass);
  }
}

void NFAState::setAccepting(BYTE anchor) {
  m_accept.setAccepting(anchor);
}

NFAState *NFAState::getSuccessor(int c) const {
  return ((m_edge == c) || ((m_edge == EDGE_CHCLASS) && m_charClass->contains(c))) ? m_next : nullptr;
}

CharacterSet &NFAState::getCharacterSet() const {
  assert(m_edge == EDGE_CHCLASS);
  return *m_charClass;
}

#if defined(_DEBUG)
String NFAState::toString() const {
  String result = format(_T("NFA state %3d "), m_id);

  if(m_next != nullptr) {
    result += format(_T("Goto %3d"), m_next->m_id);
    if(m_next2) {
      result += format(_T(" or %3d"), m_next2->m_id);
    }
    result += _T(" on ");
    switch(m_edge) {
    case EDGE_CHCLASS :
      result += charBitSetToString(*m_charClass, getFormater());
      break;
    case EDGE_EPSILON :
      result += _T("EPSILON ");
      break;
    case EDGE_UNUSED  :
      throwException(_T("NFAState::toString:m_edge==EDGE_UNUSED"));
      break;
    default           :
      result += getFormater().toString(m_edge) + _T(" ");
      break;
    }
  }

//  if( nfa == start )
//      printf(_T(" (START STATE)"));

  if(m_accept.isAccepting()) {
    result += m_accept.toString();
  }
  if(m_patternCharIndex >= 0) {
    result += format(_T(" [%d]"), m_patternCharIndex);
  }
  return result;
}

String NFAState::allAllocatedToString() { // static
  const CompactArray<NFAState*> &tmp = getAllAllocated();
  String result;
  for(size_t i = 0; i < tmp.size(); i++) {
    result += format(_T("%s\n"), tmp[i]->toString().cstr());
  }
  return result;
}

const CompactArray<NFAState*> &NFAState::getAllAllocated() { // static
  int id = 0;
  CompactArray<NFAState*> tmp;
  for(size_t i = 0; i < s_allocatedStates.size(); i++) {
    s_allocatedStates[i]->m_id = -1;
  }
  for(size_t i = 0; i < s_allocatedStates.size(); i++) {
    NFAState *p = s_allocatedStates[i];
    if(p->inUse() && (p->getID() == -1)) { // remove unused and dupletes
      p->m_id = id++;
      tmp.add(p);
    }
  }
  if(tmp.size() != s_allocatedStates.size()) {
    s_allocatedStates = tmp;
    s_allocatedStates.sort(compareById);
  }
  return s_allocatedStates;
}

#endif

int NFAState::compareById(NFAState * const &s1, NFAState * const &s2) { // static
  return s1->m_id - s2->m_id;
}
