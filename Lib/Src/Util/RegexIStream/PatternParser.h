#pragma once

#include "PatternScanner.h"
#include "SubNFA.h"

class NFA;

class PatternParser {
private:
  const bool     m_ignoreCase;
  PatternScanner m_scanner;
  PatternToken   m_token;
  NFA           &m_NFA;
  NFAStatePool  &m_statePool;

  inline void nextToken() {
    m_token = m_scanner.nextToken();
  }

  PatternParser(PatternParser &src);                  // Not defined. Class not cloneable
  PatternParser &operator=(const PatternParser &src); // Not defined. Class not cloneable

  NFAState *compilePattern();
  SubNFA    exprList();
  SubNFA    factor();
  SubNFA    term();
  SubNFA    alternativeList();
  SubNFA    escapeCommand();
  SubNFA    quest(const SubNFA &s) const {
    return s.questClosure();
  }
  SubNFA    plus( const SubNFA &s) const {
    return s.plusClosure();
  }
  SubNFA    star( const SubNFA &s) const {
    return s.starClosure();
  }
  void charClass(CharacterSet &set);
  inline bool match(PatternToken token) const {
    return m_token == token;
  }

  bool firstExprList(PatternToken token) const;
  void unexpectedEndOfPattern();
  void unexpectedInput();
  void expected(const TCHAR *input);
  void error(                 _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void error( intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void verror(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
public:
  PatternParser(const StringArray &pattern, NFA &nfa, bool ignoreCase);
};
