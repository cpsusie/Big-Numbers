#pragma once

#include "HeapObjectPool.h"
#include "BitSet.h"

#define EDGE_EPSILON  -1                              // non-character values of NFAState.m_edge
#define EDGE_CHCLASS  -2
#define EDGE_UNUSED   -3

#define NEWLINE _T('\n')
#define CR      _T('\r')

// maximal character value
#if defined(UNICODE)
#define MAX_CHARS 0x10000
#else
#define MAX_CHARS 0x100
#endif

class CharacterSet : public BitSet {
public:
  CharacterSet() : BitSet(MAX_CHARS) {
  }
  CharacterSet(const CharacterSet &src) : BitSet(src) {
  }
  CharacterSet &operator=(const BitSet &src) {
    *((BitSet*)this) = src;
    return *this;
  }
  _TUCHAR getFirst() const { // assume not empty
    return (_TUCHAR)(((BitSet&)(*this)).getIterator().next());
  }
};

class NFAState {
private:
  friend class NFAStatePool;
  friend class NFA;
  void cleanup();
  void copy(const NFAState &src);
  int           m_id         : 30; // The states id
  bool          m_startState : 1;
  bool          m_marked     : 1;  // Only true inside NFA constructor
  int           m_edge;            // Label for outgoing edge: character (>=0), EDGE_CHCLASS or EDGE_EPSILON
  CharacterSet *m_charClass;       // Characterclass when m_edge = EDGE_CHCLASS
  int           m_acceptIndex;     // if < 0, not acceptstate

  NFAState(const NFAState &src);   // Not defined. Class not cloneable

  inline void setMark(bool value) {
    m_marked = value;
  }
  inline bool isMarked() const {
    return m_marked;
  }
public:
  NFAState     *m_next;            // Next state (or nullptr if none), or tonext free if UNUSED
  NFAState     *m_next2;           // Alternative next state if m_edge = EDGE_EPSILON. nullptr if no alternative.

  NFAState();
  ~NFAState();
  NFAState &operator=(const NFAState &src);

  NFAState *getSuccessor(int c) const; // Returns successor-state on transition c (character). nullptr if none

  CharacterSet &getCharacterSet() const;
  void setEdge(int edge);
  inline int getEdge() const {
    return m_edge;
  }
  inline bool isAcceptState() const {
    return m_acceptIndex >= 0;
  }
  inline void setAcceptIndex(UINT index) {
    assert(!isAcceptState());
    m_acceptIndex = index;
  }
  inline int getAcceptIndex() const {
    return m_acceptIndex;
  }
  inline int getID() const {
    return m_id;
  }
  void setToStartState() {
    m_startState = true;
  }
  inline bool isStartState() const {
    return m_startState;
  }
  bool inUse() const {
    return m_edge != EDGE_UNUSED;
  }
  static int compareById(NFAState * const &s1, NFAState * const &s2);
};

class NFAStatePool : public HeapObjectPool<NFAState,1000> {
public:
  NFAState *fetch(int edge = EDGE_EPSILON);
  void      release(NFAState *s);
};
