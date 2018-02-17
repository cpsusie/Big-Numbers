#include "stdafx.h"
#include "PatternParser.h"

static const TCHAR *numberToBigMsg      = _T("Number too big");

PatternScanner::PatternScanner(const TCHAR *source, size_t length, const TCHAR *translateTable) {
  m_source          = (_TUCHAR*)source;
  m_end             = m_source + length;
  m_current         = m_source;
  m_lastStart       = m_currentStart = m_current;
  m_translateTable  = translateTable;
  m_inSideCharClass = m_insideRange = false;
}

PatternToken PatternScanner::setNormalChar(_TUCHAR ch) { // ch = 0 use *m_current
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
    return _EOI;
  }
  m_lastStart = m_currentStart;
  m_currentStart = m_current;

  if(m_inSideCharClass) {
    switch(*m_current) {
    case _T(']' ) : advance(); m_inSideCharClass = false; return RB;
    case _T('-' ) : advance(); return DASH;
    case _T('\\') :
      advance();
      switch(*m_current) {
      case _T('-' ) :
      case _T(']' ) :
      case _T('\\') :
        return setNormalChar();
      case 0        : unexpectedEndOfPattern();
      default       :
        return setNormalChar(_T('\\'));
      }
      break;
    case 0        : unexpectedEndOfPattern();
    default       : return setNormalChar();
    }
  } else if(m_insideRange) {
    if(_istdigit(*m_current)) {
      const intptr_t startIndex = getIndex();
      m_theNumber = 0;
      while(_istdigit(*m_current)) {
        m_theNumber = m_theNumber * 10 + (*m_current - _T('0'));
        advance();
        if(m_theNumber > 0xffff) {
          error(startIndex, numberToBigMsg);
        }
      }
      return NUMBER;
    }
    switch(*m_current) {
    case _T(',' ) : advance(); return COMMA;
    case _T('}' ) : advance(); m_insideRange = false; return RC;
    case 0        : unexpectedEndOfPattern();
    default       : unexpectedInput();
    }
  } else { // outside range and charclass
    switch(*m_current) {
    case _T('.' ) : advance(); return DOT;
    case _T('?' ) : advance(); return QUEST;
    case _T('+' ) : advance(); return PLUS;
    case _T('*' ) : advance(); return STAR;
    case _T('{' ) : advance(); m_insideRange     = true;  return LC;
    case _T('[' ) : advance(); m_inSideCharClass = true;  return LB;
    case _T('^' ) :
      if(m_current == m_source) {
        advance();
        return BEGINLINE;
      } else {
        return setNormalChar();
      }
    case _T('$' ) :
      if(m_current == m_end-1) {
        advance();
        return ENDLINE;
      } else {
        return setNormalChar();
      }
    case _T('\\') :
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
      case _T('\\') :
        setNormalChar(*m_current);
        advance();
        return NORMALCHAR;
      case 0        : unexpectedEndOfPattern();
      default       :
        return setNormalChar(_T('\\'));
/*
      case _T('1') :
      case _T('2') :
      case _T('3') :
      case _T('4') :
      case _T('5') :
      case _T('6') :
      case _T('7') :
      case _T('8') :
      case _T('9') :
        m_theRegister = *m_current - _T('0');
        advance();
        return REGISTER;
      case _T('`' ) : advance(); return BEGINBUF;
      case _T('\'') : advance(); return ENDBUF;
      case _T('<' ) : advance(); return BEGINWORD;
      case _T('>' ) : advance(); return ENDWORD;
      case _T('b' ) : advance(); return WORDBOUND;
      case _T('B' ) : advance(); return NOTWORDBOUND;
*/

      }
    default:
      return setNormalChar();
    }
  }
  throwException(_T("PatternScanner::nextToken() dropped to the end"));
  return _EOI;
}

String PatternScanner::getTokenStr() const {
  const intptr_t len = m_current - m_currentStart;
  String result = spaceString(len);
  if(len) {
    _tcsncpy(result.cstr(), (TCHAR*)m_currentStart, len);
  }
  return result;
}

BitSet PatternScanner::translate(const BitSet &set) const {
  if(!hasTranslateTable()) {
    return set;
  } else {
    BitSet result(set.getCapacity());
    for(Iterator<size_t> it = ((BitSet&)set).getIterator(); it.hasNext();) {
      result.add(translate((_TUCHAR)it.next(), m_translateTable));
    }
    return result;
  }
}

void PatternScanner::unexpectedEndOfPattern() {
  error(_T("Unexpected end of regular expression"));
}

void PatternScanner::unexpectedInput() {
  error(_T("Unexpected input:%c"), *m_current);
}

void PatternScanner::error(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(getIndex(), format, argptr);
  va_end(argptr);
}

void PatternScanner::error(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  verror(index, format, argptr);
  va_end(argptr);
}

void PatternScanner::verror(intptr_t index, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String errMsg = vformat(format, argptr);
  throwException(_T("(%d):%s"), index, errMsg.cstr());
}
