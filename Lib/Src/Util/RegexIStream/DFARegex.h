#pragma once

#include "StringArray.h"

class DFATables {
private:
  friend class DFA;
  size_t      m_memoryUsage; // in bytes
  void init();
  void copy(const DFATables &src);
  String acceptStatesToString() const;
  inline short &transition(UINT r, UINT c) {
    return m_transitionMatrix[m_columnCount*r + c];
  }
  inline const short &transition(UINT r, UINT c) const {
    return m_transitionMatrix[m_columnCount*r + c];
  }
public:
  size_t      m_stateCount;
  // number of rows in m_transitionMatrix. maybe != m_stateCount
  size_t      m_rowCount;
  // width of each row in m_transisitonMatrix
  size_t      m_columnCount;
  UINT        m_charMapSize;
  short      *m_charMap;     // size = m_charMapSize
  // size = m_stateCount
  short      *m_stateMap;
  // size = m_rowCount * m_columnCount
  short      *m_transitionMatrix;
  // size = m_stateCount
  short      *m_acceptStates; // if m_acceptState[i]>=0, then state i is an accepting state, and the returnvalue is the indexed element
  DFATables() {
    init();
  }
  DFATables(const DFATables &src);
  DFATables &operator=(const DFATables &src);
  virtual ~DFATables();

  void allocate(size_t stateCount, UINT charMapSize);
  void allocateMatrix(size_t rowCount, size_t columnCount);
  void setCharMapSize(UINT charMapSize);
  void clear();
  inline bool isEmpty() const {
    return m_stateCount == 0;
  }
  inline size_t getMemoryUsage() const {
    return m_memoryUsage;
  }
  inline int nextState(int state, _TUCHAR c) const {
    return transition(m_stateMap[state], (c < m_charMapSize) ? m_charMap[c] : 0);
  }
  inline bool isAcceptState(UINT state) const {
    return m_acceptStates[state] >= 0;
  }
  inline int getAcceptValue(UINT state) const {
    return m_acceptStates[state];
  }
  String toString() const;
};

class DFARegex {
private:
  DFATables  m_tables;
  const bool m_ignoreCase;
  void compilePattern(const StringArray &pattern);
public:
  DFARegex(const StringArray &pattern, bool ignoreCase);
  // return index of the regular expression in compiled StringArray, that matched the input, -1 if none
  // if matchedString is specified, it will contain the inputtext read from input if a match occurs
  // and will remain untouched if no match
  int match(std::istream  &in, String *matchedString) const;
  int match(std::wistream &in, String *matchedString) const;
  inline bool getIgnoreCase() const {
    return m_ignoreCase;
  }
  String toString() const;
};
