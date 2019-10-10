#pragma once

#include "SourcePosition.h"
#include "StringArray.h"

typedef enum {
  _EOL
  , NORMALCHAR
  , NUMBER
  , DOT
  , QUEST
  , PLUS
  , STAR
  , BAR
  , LPAR
  , RPAR
  , LB
  , RB
  , COMMA
  , DASH
  , WORDCHAR
  , NONWORDCHAR
} PatternToken;

#define TRANSLATE(ch, ignoreCase) ((ignoreCase&&_istlower(ch))?_totupper(ch):(ch))

class PatternScanner {
private:
  const StringArray   &m_sourceLines;
  const UINT           m_lineCount;
  UINT                 m_currentLineIndex;
  const _TUCHAR       *m_source, *m_end, *m_current, *m_lastStart, *m_currentStart;
  const bool           m_ignoreCase;
  bool                 m_insideCharClass;
  _TUCHAR              m_theCharacter;
  void   initPointers();
  void   setupPointers();
  inline void advance() {
    if(m_current < m_end) m_current++;
  }
  // ch = 0 use *m_current and advance
  PatternToken setNormalChar(_TUCHAR ch = 0);
  void unexpectedEndOfPattern();
  void unexpectedInput();
  void error(const SourcePosition &pos,  _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void error(                            _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);

public:
  PatternScanner(const StringArray &sourceLines, bool ignoreCase);
  inline UINT getCurrentLineIndex() const {
    return m_currentLineIndex;
  }
  // return true if getCurrentLineIndex() < m_lineCount - 1
  inline bool hasNextLine() const {
    return getCurrentLineIndex() < m_lineCount - 1;
  }
  // throw if(!hasMoreLines())
  void nextLine();
  // return true if end-of-line
  inline bool eol() const {
    return m_current == m_end;
  }
  // return true if end-of-source, ie eol() == true and hasNextLine() == false
  inline bool eos() const {
    return eol() && !hasNextLine();
  }
  // return line-position (index of first character) in current lexeme (token)
  inline intptr_t getIndex() const {
    return m_currentStart - m_source;
  }
  inline intptr_t getLastIndex() const {
    return m_lastStart - m_source;
  }
  inline SourcePosition getPos() const {
    return SourcePosition(getCurrentLineIndex(), (int)getIndex());
  }
  inline const TCHAR *getRest() const {
    return (TCHAR*)m_current;
  }
  PatternToken nextToken();
  // Return string containg the characters the last token consists of
  String getTokenStr() const;

  inline _TUCHAR getTheCharacter() const {
    return TRANSLATE(m_theCharacter, m_ignoreCase);
  }
  inline _TUCHAR getRawCharacter() const {
    return m_theCharacter;
  }
  BitSet translate(const BitSet &set) const;
};
