#include "stdafx.h"
#include "NFA.h"

#ifdef _DEBUG

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
CharacterFormater      *NFAState::s_formater   = CharacterFormater::hexEscapedExtendedAsciiFormater;
CompactArray<NFAState*> NFAState::s_allocatedStates;

#define incrCount() s_stateCount++
#define decrCount() s_stateCount--

#else

#define incrCount()
#define decrCount()

#endif

#define STATESPERPAGE 2000

class NFAStatePage {
private:
  friend class NFAStateManager;
  NFAStatePage *m_next;
public:
  NFAState m_stateArray[STATESPERPAGE];

  NFAStatePage();
  ~NFAStatePage();
};

class NFAStateManager {
public:
  NFAStatePage *m_firstPage;
  NFAState     *m_firstState;

  NFAStateManager();
  ~NFAStateManager() {
    deleteAll();
  }
  void deleteAll();
};

NFAStateManager::NFAStateManager() {
  m_firstPage  = NULL;
  m_firstState = NULL;
}

void NFAStateManager::deleteAll() {
  for(NFAStatePage *p = m_firstPage, *q = NULL; p; p = q) {
    q = p->m_next;
    delete p;
  }
  m_firstPage  = NULL;
  m_firstState = NULL;
}

static NFAStateManager stateManager;

NFAStatePage::NFAStatePage() {
  NFAState *p = &LASTVALUE(m_stateArray);
  p->m_next1 = stateManager.m_firstState;
  for(NFAState *q = p-1; q >= m_stateArray; p = q--) {
    q->m_next1 = p;
  }
  stateManager.m_firstState = m_stateArray;

  m_next = stateManager.m_firstPage;
  stateManager.m_firstPage = this;
}

NFAStatePage::~NFAStatePage() {
  stateManager.m_firstPage = m_next;
}

NFAState *NFAState::fetch(int edge) {  // static
  if(stateManager.m_firstState == NULL) {
    new NFAStatePage;
  }
  NFAState *s = stateManager.m_firstState;
  stateManager.m_firstState = s->m_next1;
  s->m_next1 = NULL;
  assert(s->m_edge == EDGE_UNUSED);
  assert(!s->isMarked());
  s->setEdge(edge);

#ifdef _DEBUG
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
  s->m_next1 = stateManager.m_firstState;
  stateManager.m_firstState = s;
}

void NFAState::releaseAll() {          // static
#ifdef _DEBUG
  s_allocatedStates.clear();
#endif
  stateManager.deleteAll();
}

NFAState::NFAState() { // private
  m_id         = -1;
  m_startState = false;
  m_marked     = false;
  m_edge       = EDGE_UNUSED;
  m_charClass  = NULL;
  m_next1      = m_next2 = NULL;
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
  m_next1      = src.m_next1;
  m_next2      = src.m_next2;

  if(src.m_charClass) {
    assert(src.m_edge == EDGE_CHCLASS);
    m_charClass = new CharacterSet(*src.m_charClass);
  } else {
    assert(src.m_edge != EDGE_CHCLASS);
    m_charClass = NULL;
  }
#ifdef _DEBUG
  m_patternCharIndex = src.m_patternCharIndex;
#endif
}

void NFAState::cleanup() {
  assert(!isMarked());
  if(m_charClass) {
    assert(m_edge == EDGE_CHCLASS);
    delete m_charClass;
    m_charClass = NULL;
  } else {
    assert(m_edge != EDGE_CHCLASS);
  }

  m_id                       = -1;
  m_startState               = false;
  m_edge                     = EDGE_EPSILON;
  m_next1                    = m_next2 = NULL;
  m_accept.m_acceptAttribute = 0;
}

void NFAState::setEdge(int edge) {
  if(edge != m_edge) {
    if(m_edge == EDGE_CHCLASS) {
      delete m_charClass;
      m_charClass = NULL;
    }
  }
  m_edge = edge;
  if(m_edge == EDGE_CHCLASS) {
    m_charClass = new CharacterSet;
  }
}

void NFAState::setAccepting(BYTE anchor) {
  m_accept.setAccepting(anchor);
}

NFAState *NFAState::getSuccessor(int c) const { 
  return ((m_edge == c) || ((m_edge == EDGE_CHCLASS) && m_charClass->contains(c))) ? m_next1 : NULL;
}

CharacterSet &NFAState::getCharacterSet() const {
  assert(m_edge == EDGE_CHCLASS);
  return *m_charClass;
}

#ifdef _DEBUG
String NFAState::toString() const {
  String result = format(_T("NFA state %3d "), m_id);

  if(m_next1 != NULL) {
    result += format(_T("Goto %3d"), m_next1->m_id);
    if(m_next2) {
      result += format(_T(" or %3d"), m_next2->m_id);
    }
    result += _T(" on ");
    switch(m_edge) {
      case EDGE_CHCLASS :
        result += charBitSetToString(*m_charClass, s_formater);
        break;
      case EDGE_EPSILON :
        result += _T("EPSILON ");
        break;
      case EDGE_UNUSED  :
        throwException(_T("NFAState::toString:m_edge==EDGE_UNUSED"));
        break;
      default           :
        result += s_formater->toString(m_edge) + _T(" ");
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
