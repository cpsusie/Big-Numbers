#pragma once

#include "NFA.h"

#if defined(_DEBUG)
#include <Stack.h>
#endif

typedef enum {
  _EOI
 ,NORMALCHAR
 ,NUMBER
 ,DOT
 ,QUEST
 ,PLUS
 ,STAR
 ,BAR
 ,LPAR
 ,RPAR
 ,LB
 ,RB
 ,LC
 ,RC
 ,COMMA
 ,DASH
// ,BEGINBUF
// ,ENDBUF
 ,BEGINLINE
 ,ENDLINE
// ,BEGINWORD
// ,ENDWORD
 ,WORDCHAR
 ,NONWORDCHAR
// ,WORDBOUND
// ,NOTWORDBOUND
// ,REGISTER
} PatternToken;

#define TRANSLATE(ch) (m_translateTable ? PatternScanner::translate(ch, m_translateTable) : ((_TUCHAR)(ch)))

class PatternScanner {
private:
  const _TUCHAR       *m_source, *m_end, *m_current, *m_lastStart, *m_currentStart;
  const TCHAR         *m_translateTable;
  UINT                 m_theNumber;
  BYTE                 m_theRegister;
  bool                 m_inSideCharClass, m_insideRange;
  _TUCHAR              m_theCharacter;
  inline void advance() {
    if(m_current < m_end) m_current++;
  }
  // ch = 0 use *m_current and advance
  PatternToken setNormalChar(_TUCHAR ch = 0);
  void unexpectedEndOfPattern();
  void unexpectedInput();
  void error(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void error(_In_z_ _Printf_format_string_ TCHAR const * const format,...);
  void verror(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

public:
  PatternScanner(const TCHAR *source, size_t length, const TCHAR *translateTable);
  inline bool eos() const {
    return m_current == m_end;
  }
  inline intptr_t getIndex() const {
    return m_currentStart - m_source;
  }
  inline intptr_t getLastIndex() const {
    return m_lastStart - m_source;
  }
  inline const TCHAR *getRest() const {
    return (TCHAR*)m_current;
  }
  PatternToken nextToken();
  // Return string containg the characters the last token consists of
  String getTokenStr() const;

  inline unsigned short getTheNumber() const {
    return m_theNumber;
  }

  inline bool hasTranslateTable() const {
    return m_translateTable != nullptr;
  }

  inline _TUCHAR getTheCharacter() const {
    return TRANSLATE(m_theCharacter);
  }

  inline _TUCHAR getRawCharacter() const {
    return m_theCharacter;
  }

  static inline _TUCHAR translate(_TUCHAR ch, const TCHAR *table) {
    return (ch < 256)? table[ch] : ch;
  }

  BitSet translate(const BitSet &set) const;
};

class PatternParser {
private:
  PatternScanner m_scanner;
  PatternToken   m_token;
  NFA           &m_NFA;

#if defined(_DEBUG)
  Stack<String>  m_callStack;

  String    buildCodeString() const;
  void      enter(const TCHAR *function);
  void      leave(const TCHAR *function);
  void      nextToken();
#else
  inline void nextToken() {
    m_token = m_scanner.nextToken();
  }
#endif

  PatternParser(PatternParser &src);                  // Not defined. Class not cloneable
  PatternParser &operator=(const PatternParser &src); // Not defined. Class not cloneable

  NFAState *compilePattern();
  SubNFA exprList();
  SubNFA factor();
  SubNFA term();
  SubNFA alternativeList();
  SubNFA escapeCommand();
  SubNFA quest(const SubNFA &s) const {
    return s.questClosure();
  }

  SubNFA plus( const SubNFA &s) const {
    return s.plusClosure();
  }
;
  SubNFA star( const SubNFA &s) const {
    return s.starClosure();
  }

  SubNFA range(const SubNFA &s);
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
  PatternParser(const String &pattern, NFA &nfa, const TCHAR *translateTable = nullptr);
  void thompsonConstruction();
};
