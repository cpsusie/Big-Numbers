#pragma once

#include "NFAState.h"

class SubNFA {
private:
  NFAStatePool &m_statePool;
  NFAState     *m_start;
  NFAState     *m_end;
public:
  SubNFA(NFAStatePool &statePool, bool init = false) : m_statePool(statePool) {
    m_start = init ? m_statePool.fetch() : NULL;
    m_end   = init ? m_statePool.fetch() : NULL;
  }
  SubNFA &operator=(const SubNFA &src) {
    m_start = src.m_start;
    m_end = src.m_end;
    return *this;
  }
  SubNFA &create2StateNFA(_TUCHAR ch);      // create an NFA with a startstate and end endstate with transition from start to on edge
  SubNFA &create2StateNFA(const CharacterSet &charSet);
  SubNFA &createEpsilon();                  // creates a 1 state NFA with an epsilon transition from start to end
  SubNFA clone() const;
  SubNFA &operator+=(      SubNFA &s);      // concatenation releases s.m_start
  SubNFA &operator|=(const SubNFA &s);      // adds alternative s to this
  SubNFA questClosure() const;
  SubNFA plusClosure()  const;
  SubNFA starClosure()  const;
  inline bool isEmpty() const {
    return m_start == NULL;
  }
  inline void setAcceptIndex(UINT index) {
    m_end->setAcceptIndex(index);
  }
  inline NFAState *getStart() {
    return m_start;
  }
};
