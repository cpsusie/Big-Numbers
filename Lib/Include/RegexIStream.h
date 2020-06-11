#pragma once

#include "StringArray.h"

class DFARegex;

class RegexIStream {
private:
  DFARegex *m_regex;
  RegexIStream(const RegexIStream &);            // Not defined. Class not cloneable
  RegexIStream &operator=(const RegexIStream &); // Not defined. Class not cloneable
  void cleanup();
public:
  RegexIStream() : m_regex(NULL) {
  }
#if defined(_DEBUG)
  RegexIStream(const StringArray &pattern, bool ignoreCase = false, bool dumpStates = true) : m_regex(NULL) {
    compilePattern(pattern, ignoreCase,dumpStates);
  }
  void compilePattern(const StringArray &pattern, bool ignoreCase = false, bool dumpStates = true);
#else
  void compilePattern(const StringArray &pattern, bool ignoreCase = false);
  RegexIStream(const StringArray &pattern, bool ignoreCase = false) : m_regex(NULL) {
    compilePattern(pattern, ignoreCase);
  }
#endif // _DEBUG
  virtual ~RegexIStream() {
    cleanup();
  }
  inline bool isEmpty() const {
    return m_regex == NULL;
  }
  int match(std::istream  &in, String *matchedString = NULL) const;
  int match(std::wistream &in, String *matchedString = NULL) const;
#if defined(_DEBUG)
  String toString() const;
#endif // _DEBUG
};
