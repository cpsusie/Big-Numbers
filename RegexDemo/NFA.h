#pragma once

#include <HeapObjectPool.h>

#define EDGE_EPSILON  -1                              // non-character values of NFAState.m_edge
#define EDGE_CHCLASS  -2
#define EDGE_UNUSED   -3

#define ANCHOR_NONE  0                           //  Not anchored
#define ANCHOR_START 0x1                         //  Pattern Anchored at start of line
#define ANCHOR_END   0x2                         //  At end of line
#define ACCEPT_STATE 0x4

#define ANCHOR_BOTH  (ANCHOR_START | ANCHOR_END) //  Both start and end of line.

#define NEWLINE _T('\n')
#define CR      _T('\r')

// maximal character value
#if defined(UNICODE)
#define MAX_CHARS 0x10000
#else
#define MAX_CHARS 0x100
#endif

class AcceptType {
public:
  BYTE m_acceptAttribute;
  AcceptType() : m_acceptAttribute(0) {
  }

  inline bool isAccepting() const {
    return m_acceptAttribute != 0;
  }

  inline bool operator==(const AcceptType &a) const {
    return m_acceptAttribute == a.m_acceptAttribute;
  }

  inline bool operator!=(const AcceptType &a) const {
    return !(*this == a);
  }

  inline void setAccepting(BYTE anchor) {
    m_acceptAttribute |= anchor | ACCEPT_STATE;
  }

#if defined(_DEBUG)
  String toString() const;
#endif
};

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
#if defined(_DEBUG)
  static int                     s_stateCount;
  static CharacterFormater      *s_formater;
  static CompactArray<NFAState*> s_allocatedStates;

#endif
  static HeapObjectPool<NFAState> s_stateManager;
  friend class NFA;
  void cleanup();
  void copy(const NFAState &src);
  int           m_id         : 30; // The states id
  bool          m_startState :  1;
  bool          m_marked     :  1; // Only true inside NFA constructor
  int           m_edge;            // Label for outgoing edge: character (>=0), EDGE_CHCLASS or EDGE_EPSILON
  CharacterSet *m_charClass;       // Characterclass when m_edge = EDGE_CHCLASS

  NFAState(const NFAState &src);   // Not defined. Class not cloneable

  inline void setMark(bool value) {
    m_marked = value;
  }
  inline bool isMarked() const {
    return m_marked;
  }
public:
  NFAState     *m_next;            // Next state (or NULL if none), or tonext free if UNUSED
  NFAState     *m_next2;           // Alternative next state if m_edge = EDGE_EPSILON. NULL if no alternative.
  AcceptType    m_accept;

  static NFAState *fetch(int edge = EDGE_EPSILON);
  static void      release(NFAState *s);
  static void      releaseAll();

  NFAState();
  ~NFAState();
  NFAState &operator=(const NFAState &src);

  NFAState *getSuccessor(int c) const; // Returns successor-state on transition c (character). NULL if none

  CharacterSet &getCharacterSet() const;
  void setEdge(int edge);
  inline int getEdge() const {
    return m_edge;
  }
  void setAccepting(BYTE anchor);
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

#if defined(_DEBUG)
  String toString() const;
  int m_patternCharIndex;
  static String allAllocatedToString();
  static const CompactArray<NFAState*> &getAllAllocated();

  static void setFormater(CharacterFormater *formater) {
    NFAState::s_formater = formater;
  }
  static CharacterFormater *getFormater() {
    return s_formater;
  }
  static int getAllocated() {
    return s_stateCount;
  }
#endif
};

#if defined(_DEBUG)

class DFARegexStepHandler;

#define _DBG_callCompilerHandler(handler, codeString, stackString, fastMapStr, scannerIndex)                  \
{ if(handler) {                                                                                               \
    handler->handleCompileStep(_DFARegexCompilerState(codeString                                              \
                                                     ,stackString                                             \
                                                     ,fastMapStr                                              \
                                                     ,scannerIndex                                            \
                                                     )                                                        \
                              );                                                                              \
  }                                                                                                           \
}

#define DBG_callCompilerHandler(codeString, stackString, fastMapStr, scannerIndex)                            \
{ _DBG_callCompilerHandler(m_NFA.m_stepHandler, codeString, stackString, fastMapStr, scannerIndex)            \
}

#define DBG_callSearchHandler(startPos, charIndex)                                                            \
{ if(m_stepHandler) { m_stepHandler->handleSearchStep(_DFARegexSearchState(startPos, charIndex)); } }

#define DBG_callMatchHandler()                                                                                \
{ if(m_stepHandler) {                                                                                         \
    m_stepHandler->handleMatchStep(_DFARegexMatchState(this                                                   \
                                                      ,m_currentState                                         \
                                                      ,m_lastAcceptState                                      \
                                                      ,pos                                                    \
                                                      ,cp-(_TUCHAR*)string                                    \
                                                      ,(m_lastAcceptState < 0)                                \
                                                       ? -1                                                   \
                                                       : ((const TCHAR*)matchEndp-string)                     \
                                                       )                                                      \
                                  );                                                                          \
  }                                                                                                           \
}

#else

#define _DBG_callCompilerHandler(handler, codeString, stackString, fastMapStr, scannerIndex)
#define DBG_callCompilerHandler(codeString, stackString, fastMapStr, scannerIndex)
#define DBG_callSearchHandler(startPos, charIndex)
#define DBG_callMatchHandler()

#endif

class SubNFA {
private:
  NFAState *m_start;
  NFAState *m_end;
public:
  SubNFA(bool init = false) : m_start(init? NFAState::fetch() : NULL), m_end(init ? NFAState::fetch() : NULL) {
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
  bool isEmpty() const {
    return m_start == NULL;
  }
  void setAccepting(BYTE anchor) {
    m_end->setAccepting(anchor);
  }
  NFAState *getStart() {
    return m_start;
  }
#if defined(_DEBUG)
  void setCharIndex(intptr_t start, intptr_t end);
  String toString() const;
#endif
};

class NFA : public CompactArray<NFAState*> {
private:
  int m_idCounter;

  NFA(const NFA &src);      // Not defined. Class not cloneable
  NFA &operator=(NFA &src); // Not defined. Class not cloneable

  void        addIfNotMarked(NFAState *s);
  static void unmarkAll(     NFAState *s);
public:
#if defined(_DEBUG)
  DFARegexStepHandler *m_stepHandler;
  NFA(DFARegexStepHandler *stepHandler = NULL) : m_stepHandler(stepHandler) {
  }
#else
  NFA() {
  }
#endif
  NFA(NFAState *start);
  void create(NFAState *start);
  ~NFA(); // All elements in array will be released
          // clear and remove will NOT release the elements

#if defined(_DEBUG)
  String toString() const;
  int getMaxCharIndex() const;
#endif
};

#if defined(_DEBUG)
void NFAPaint(           CWnd *wnd, CDC &dc);
void NFAAnimateBuildStep(CWnd *wnd);
#endif
