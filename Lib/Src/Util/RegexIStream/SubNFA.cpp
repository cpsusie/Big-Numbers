#include "pch.h"
#include "NFA.h"
#include "SubNFA.h"

SubNFA SubNFA::clone() const {
  SubNFA result(m_statePool);
  if(isEmpty()) {
    return result;
  }

  NFA srcNFA(m_statePool, m_start);
  NFA newNFA(m_statePool);

  for(size_t i = 0; i < srcNFA.size(); i++) {
    NFAState *s = srcNFA[i];
    NFAState *s1 = m_statePool.fetch(s->getEdge());
    *s1 = *s;
    newNFA.add(s1);
  }

  result.m_start = newNFA[0];
  result.m_end = newNFA[m_end->getID()];;
  for(size_t i = 0; i < srcNFA.size(); i++) {
    NFAState *s1 = srcNFA[i];
    NFAState *s2 = newNFA[i];
    if(s1->m_next) s2->m_next = newNFA[s1->m_next->getID()];
    if(s2->m_next2) s2->m_next2 = newNFA[s1->m_next2->getID()];
  }
  newNFA.clear();
  srcNFA.clear();
  return result;
}

SubNFA &SubNFA::create2StateNFA(_TUCHAR ch) {
  m_start = m_statePool.fetch(ch);
  m_end = m_start->m_next = m_statePool.fetch();
  return *this;
}

SubNFA &SubNFA::create2StateNFA(const CharacterSet &charSet) {
  m_start = m_statePool.fetch(EDGE_CHCLASS);
  m_end = m_start->m_next = m_statePool.fetch();
  m_start->getCharacterSet() = charSet;
  return *this;
}

SubNFA &SubNFA::operator+=(SubNFA &s) {
  if(isEmpty()) {
    *this = s;
  } else if(!s.isEmpty()) {
    *m_end = *s.m_start;
    m_statePool.release(s.m_start);
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

  NFAState *p = m_statePool.fetch(EDGE_EPSILON);
  p->m_next2 = t.m_start;
  p->m_next = m_start;
  m_start = p;

  p = m_statePool.fetch(EDGE_EPSILON);
  m_end->m_next = p;
  t.m_end->m_next = p;
  m_end = p;
  return *this;
}

SubNFA &SubNFA::createEpsilon() {
  m_start = m_statePool.fetch(EDGE_EPSILON); // make an epsilon-transition directly from start to end
  m_end = m_start;
  m_start->m_next = m_end;
  return *this;
}

SubNFA SubNFA::questClosure() const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(m_statePool, true);

  result.m_start->m_next = m_start;
  m_end->m_next = result.m_end;
  result.m_start->m_next2 = result.m_end;
  return result;
}

SubNFA SubNFA::plusClosure() const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(m_statePool, true);

  result.m_start->m_next = m_start;
  m_end->m_next = result.m_end;
  m_end->m_next2 = m_start;
  return result;
}

SubNFA SubNFA::starClosure() const {
  if(isEmpty()) {
    return *this;
  }
  SubNFA result(m_statePool, true);

  result.m_start->m_next = m_start;
  m_end->m_next = result.m_end;
  result.m_start->m_next2 = result.m_end;
  m_end->m_next2 = m_start;
  return result;
}
