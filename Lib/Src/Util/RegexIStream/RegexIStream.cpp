#include "pch.h"
#include "DFARegex.h"
#include <RegexIStream.h>

using namespace std;

#if defined(_DEBUG)
void RegexIStream::compilePattern(const StringArray &pattern, bool ignoreCase, bool dumpStates) {
  cleanup();
  BYTE flags = 0;
  if(ignoreCase) flags |= DFA_IGNORECASE;
  if(dumpStates) flags |= DFA_DUMPSTATES;
  m_regex = new DFARegex(pattern, flags); TRACE_NEW(m_regex);
}
#else
void RegexIStream::compilePattern(const StringArray &pattern, bool ignoreCase) {
  cleanup();
  BYTE flags = 0;
  if(ignoreCase) flags |= DFA_IGNORECASE;
  m_regex = new DFARegex(pattern, flags); TRACE_NEW(m_regex);
}
#endif // _DEBUG

void RegexIStream::cleanup() {
  SAFEDELETE(m_regex);
}

int RegexIStream::match(istream &in, String *matchedString) const {
  if(isEmpty()) {
    throwException(_T("%s:No regular expression to match"), __TFUNCTION__);
  }
  return m_regex->match(in, matchedString);
}

int RegexIStream::match(wistream &in, String *matchedString) const {
  if(isEmpty()) {
    throwException(_T("%s:No regular expression to match"), __TFUNCTION__);
  }
  return m_regex->match(in, matchedString);
}

#if defined(_DEBUG)
String RegexIStream::toString() const {
  return isEmpty() ? EMPTYSTRING : m_regex->toString();
}
#endif // _DEBUG
