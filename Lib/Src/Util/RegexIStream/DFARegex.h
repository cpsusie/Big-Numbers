#pragma once

#include <StringArray.h>
#include <FlagTraits.h>
#include "FixedIntArray.h"

class DFATables {
private:
  friend class DFA;
  void init();
  void copy(const DFATables &src);
#if defined(_DEBUG)
  String acceptStatesToString() const;
#endif // _DEBUG

  inline const int transition(UINT r, UINT c) const {
    return (*m_transitionMatrix)[m_columnCount*r + c];
  }
public:
  UINT           m_stateCount;
  UINT           m_rowCount;         // number of rows in m_transitionMatrix. maybe != m_stateCount
  UINT           m_columnCount;      // width of each row in m_transisitonMatrix
  UINT           m_charMapSize;
  FixedIntArray *m_charMap;          // size = m_charMapSize
  FixedIntArray *m_stateMap;         // size = m_stateCount
  FixedIntArray *m_transitionMatrix; // size = m_rowCount * m_columnCount
  FixedIntArray *m_acceptStates;     // size = m_stateCount. if m_acceptState[i]>=0, then state i is an accepting state, and the returnvalue is the indexed element
  DFATables() {
    init();
  }
  DFATables(const DFATables &src);
  DFATables &operator=(const DFATables &src);
  virtual ~DFATables();
  void clear();
  inline bool isEmpty() const {
    return m_stateCount == 0;
  }
  inline int nextState(UINT state, _TUCHAR c) const {
    return transition((*m_stateMap)[state], (c < m_charMapSize) ? (*m_charMap)[c] : 0);
  }
  inline int getAcceptValue(UINT state) const {
    return (*m_acceptStates)[state];
  }
  inline bool isAcceptState(UINT state) const {
    return getAcceptValue(state) >= 0;
  }
#if defined(_DEBUG)
  UINT getMemoryUsage() const;
  String toString() const;
#endif // _DEBUG
};

#define DFA_IGNORECASE 0x01
#if defined(_DEBUG)
#define DFA_DUMPSTATES 0x02
#else
#define DFA_DUMPSTATES 0
#endif

class DFARegex {
private:
  DFATables  m_tables;
  FLAGTRAITS(DFARegex, BYTE, m_flags);
  void compilePattern(const StringArray &pattern);
public:
  DFARegex(const StringArray &pattern, BYTE flags);
  // return index of the regular expression in compiled StringArray, that matched the input, -1 if none
  // if matchedString is specified, it will contain the inputtext read from input if a match occurs
  // and will remain untouched if no match
  int match(std::istream  &in, String *matchedString) const;
  int match(std::wistream &in, String *matchedString) const;
  inline bool getIgnoreCase() const {
    return isSet(DFA_IGNORECASE);
  }
#if defined(_DEBUG)
  inline bool getDumpStates() const {
    return isSet(DFA_DUMPSTATES);
  }
  String toString() const;
#endif // _DEBUG
};
