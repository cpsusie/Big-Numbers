#include "stdafx.h"
#include "NFA.h"

NFA::NFA(NFAState *start) {
#if defined(_DEBUG)
  m_stepHandler = NULL;
#endif
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
    NFAState::release((*this)[i]);
  }
  clear();
}

#if defined(_DEBUG)
String NFA::toString() const {
  String result;
  for(size_t i = 0; i < size(); i++) {
    result += format(_T("%s\n"), (*this)[i]->toString().cstr());
  }
  return result;
}

int NFA::getMaxCharIndex() const {
  int m = -1;
  for(size_t i = 0; i < size(); i++) {
    const NFAState *s = (*this)[i];
    if(s->m_patternCharIndex > m) {
      m = s->m_patternCharIndex;
    }
  }
  return m;
}

#endif

SubNFA SubNFA::clone() const {
  SubNFA result;
  if(isEmpty()) {
    return result;
  }

  NFA srcNFA(m_start);
  NFA newNFA;

  for(size_t i = 0; i < srcNFA.size(); i++) {
    NFAState *s  = srcNFA[i];
    NFAState *s1 = NFAState::fetch(s->getEdge());
    *s1 = *s;
    newNFA.add(s1);
  }

  result.m_start = newNFA[0];
  result.m_end   = newNFA[m_end->getID()];;
  for(size_t i = 0; i < srcNFA.size(); i++) {
    NFAState *s1 = srcNFA[i];
    NFAState *s2 = newNFA[i];
    if(s1->m_next ) s2->m_next  = newNFA[s1->m_next->getID() ];
    if(s2->m_next2) s2->m_next2 = newNFA[s1->m_next2->getID()];
  }
  newNFA.clear();
  srcNFA.clear();
  return result;

//  debugLog(_T("src:\n%sclone:\n%s\n"), src.toString().cstr(), toString().cstr());
}

SubNFA &SubNFA::create2StateNFA(_TUCHAR ch) {
  m_start = NFAState::fetch(ch);
  m_end   = m_start->m_next = NFAState::fetch();
  return *this;
}

SubNFA &SubNFA::create2StateNFA(const CharacterSet &charSet) {
  m_start = NFAState::fetch(EDGE_CHCLASS);
  m_end   = m_start->m_next = NFAState::fetch();
  m_start->getCharacterSet() = charSet;
  return *this;
}

SubNFA &SubNFA::operator+=(SubNFA &s) {
  if(isEmpty()) {
    *this = s;
  } else if(!s.isEmpty()) {
    *m_end = *s.m_start;
    NFAState::release(s.m_start);
    m_end = s.m_end;
  }
  return *this;
}

SubNFA &SubNFA::operator|=(const SubNFA &s) {
  SubNFA t = s;
  if(isEmpty()) {
    createEpsilon();
  } else if(t.isEmpty()) {
    t.createEpsilon();
  }

  NFAState *p       = NFAState::fetch(EDGE_EPSILON);
  p->m_next2        = t.m_start;
  p->m_next         = m_start;
  m_start           = p;

  p                 = NFAState::fetch(EDGE_EPSILON);
  m_end->m_next     = p;
  t.m_end->m_next   = p;
  m_end             = p;
  return *this;
}

SubNFA &SubNFA::createEpsilon() {
  m_start         = NFAState::fetch(EDGE_EPSILON); // make an epsilon-transition directly from start to end
  m_end           = m_start;
  m_start->m_next = m_end;
  return *this;
}

SubNFA SubNFA::questClosure() const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(true);

  result.m_start->m_next  = m_start;
  m_end->m_next           = result.m_end;
  result.m_start->m_next2 = result.m_end;
  return result;
}

SubNFA SubNFA::plusClosure()  const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(true);

  result.m_start->m_next = m_start;
  m_end->m_next          = result.m_end;
  m_end->m_next2         = m_start;
  return result;
}

SubNFA SubNFA::starClosure()  const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(true);

  result.m_start->m_next  = m_start;
  m_end->m_next           = result.m_end;
  result.m_start->m_next2 = result.m_end;
  m_end->m_next2          = m_start;
  return result;
}

#if defined(_DEBUG)

void SubNFA::setCharIndex(intptr_t startIndex, intptr_t endIndex) {
  if(!isEmpty()) {
    m_start->m_patternCharIndex = (int)startIndex;
    m_end->m_patternCharIndex   = (int)endIndex;
  }
}

String SubNFA::toString() const {
  NFA tmpNFA(m_start);
  String result = tmpNFA.toString();
  tmpNFA.clear();
  result += format(_T("Start:%d End:%d\n"), m_start->getID(), m_end->getID());
  return result;
}
#endif
