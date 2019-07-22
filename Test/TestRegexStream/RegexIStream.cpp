#include "stdafx.h"
#include "DFARegex.h"
#include "RegexIStream.h"

void RegexIStream::compilePattern(const StringArray &pattern, bool ignoreCase) {
  cleanup();
  m_regex = new DFARegex(pattern, ignoreCase); TRACE_NEW(m_regex);
}

void RegexIStream::cleanup() {
  SAFEDELETE(m_regex);
}

int RegexIStream::match(std::istream &in, String *matchedString) const {
  if(isEmpty()) {
    throwException(_T("%s:No regular expression to match"), __TFUNCTION__);
  }
  return m_regex->match(in, matchedString);
}

String RegexIStream::toString() const {
  return isEmpty() ? EMPTYSTRING : m_regex->toString();
}
