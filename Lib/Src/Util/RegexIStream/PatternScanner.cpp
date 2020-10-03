 #include "pch.h"
#include "PatternScanner.h"

PatternScanner::PatternScanner(const StringArray &sourceLines, bool ignoreCase)
: m_sourceLines(sourceLines)
, m_lineCount((UINT)sourceLines.size())
, m_ignoreCase(ignoreCase)
, m_currentLineIndex(0)
{
  if(m_currentLineIndex < m_lineCount) {
    setupPointers();
  } else {
    initPointers();
  }
}

void PatternScanner::initPointers() {
  m_source          = NULL;
  m_end             = NULL;
  m_current         = NULL;
  m_lastStart       = NULL;
  m_insideCharClass = false;
}

void PatternScanner::setupPointers() {
  const String &line = m_sourceLines[getCurrentLineIndex()];
  m_source          = (_TUCHAR*)line.cstr();
  m_end             = m_source + line.length();
  m_current         = m_source;
  m_lastStart       = m_currentStart = m_current;
  m_insideCharClass = false;
}

void PatternScanner::nextLine() {
  if(!hasNextLine()) {
    throwException(_T("%s:No next line. current line=%u, linecount=%u"), __TFUNCTION__, getCurrentLineIndex(), m_lineCount);
  }
  m_currentLineIndex++;
  setupPointers();
}

PatternToken PatternScanner::setNormalChar(_TUCHAR ch) {
  if(ch == 0) {
    m_theCharacter = *m_current;
    advance();
  } else {
    m_theCharacter = ch;
  }
  return NORMALCHAR;
}

PatternToken PatternScanner::nextToken() {
  if(m_current >= m_end) {
    return _EOL;
  }
  m_lastStart    = m_currentStart;
  m_currentStart = m_current;

  if(m_insideCharClass) {
    switch(*m_current) {
    case _T(']' ) :
      advance();
      m_insideCharClass = false;
      return RB;
    case _T('-' ) :
      advance();
      return DASH;
    case _T('/') :
      advance();
      switch(*m_current) {
      case _T('-' ) :
      case _T(']' ) :
      case _T('/') :
        return setNormalChar();
      case 0        :
        unexpectedEndOfPattern();
      default       :
        return setNormalChar(_T('/'));
      }
      break;
    case 0        :
      unexpectedEndOfPattern();
    default       :
      return setNormalChar();
    }
  } else { // outside charclass
    switch(*m_current) {
    case _T('.' ) : advance(); return DOT;
    case _T('?' ) : advance(); return QUEST;
    case _T('+' ) : advance(); return PLUS;
    case _T('*' ) : advance(); return STAR;
    case _T('[' ) :
      advance();
      m_insideCharClass = true;
      return LB;
    case _T('/') :
      advance();
      switch(*m_current) {
      case _T('(' ) : advance(); return LPAR;
      case _T(')' ) : advance(); return RPAR;
      case _T('|' ) : advance(); return BAR;
      case _T('w' ) : advance(); return WORDCHAR;
      case _T('W' ) : advance(); return NONWORDCHAR;
      case _T('.' ) :
      case _T('?' ) :
      case _T('+' ) :
      case _T('*' ) :
      case _T('[' ) :
      case _T(']' ) :
      case _T('/') :
        setNormalChar(*m_current);
        advance();
        return NORMALCHAR;
      case 0        :
        unexpectedEndOfPattern();
      default       :
        return setNormalChar(_T('/'));
      }
    default:
      return setNormalChar();
    }
  }
  throwException(_T("%s dropped to the end"), __TFUNCTION__);
  return _EOL;
}

String PatternScanner::getTokenStr() const {
  const TCHAR save = *m_current;
  (*(TCHAR*)m_current) = '\0';
  const String result = m_currentStart;
  (*(TCHAR*)m_current) = save;
  return result;
}

BitSet PatternScanner::translate(const BitSet &set) const {
  if(m_ignoreCase) {
    return set;
  } else {
    BitSet result(set.getCapacity());
    for(Iterator<size_t> it = set.getIterator(); it.hasNext();) {
      const _TUCHAR ch = (_TUCHAR)it.next();
      result.add(TRANSLATE(ch,true));
    }
    return result;
  }
}

void PatternScanner::unexpectedEndOfPattern() {
  if(m_insideCharClass) {
    error(_T("Unmatched '[' defining charclass"));
  } else {
    error(_T("Unexpected end of regular expression"));
  }
}

void PatternScanner::unexpectedInput() {
  error(_T("Unexpected input:%c"), *m_current);
}

void PatternScanner::error(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(getPos(), format, argptr);
  va_end(argptr);
}

void PatternScanner::error(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(pos, format, argptr);
  va_end(argptr);
}

void PatternScanner::verror(const SourcePosition &pos, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String errMsg = vformat(format, argptr);
  throwException(_T("(%s):%s"), pos.toString().cstr(), errMsg.cstr());
}
