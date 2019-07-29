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
  RegexIStream(const StringArray &pattern, bool ignoreCase = false) : m_regex(NULL) {
    compilePattern(pattern, ignoreCase);
  }
  void compilePattern(const StringArray &pattern, bool ignoreCase = false);
  virtual ~RegexIStream() {
    cleanup();
  }
  inline bool isEmpty() const {
    return m_regex == NULL;
  }
  int match(std::istream  &in, String *matchedString = NULL) const;
  int match(std::wistream &in, String *matchedString = NULL) const;
  String toString() const;
};
