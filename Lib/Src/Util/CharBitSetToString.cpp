#include "pch.h"

#define FORMATCHAR(ch) charFormater->formatChar(ch)

#define FLUSHRANGE()                                                                \
{ if(delim) result += delim; else delim = _T(",");                                  \
  if(first == last) {                                                               \
    result += FORMATCHAR(first);                                                    \
  } else {                                                                          \
    const TCHAR *formatStr = (first + 1 == last) ? _T("%s%s") : _T("%s-%s");        \
    result += format(formatStr, FORMATCHAR(first).cstr(), FORMATCHAR(last).cstr()); \
  }                                                                                 \
}

String charBitSetToString(const BitSet &set, CharacterFormater *charFormater) {
  String result = _T("[");
  _TUCHAR first=1,last=0;
  const TCHAR *delim = NULL;
  for(Iterator<size_t> it = ((BitSet&)set).getIterator(); it.hasNext();) {
    const _TUCHAR ch = (_TUCHAR)it.next();
    if(first > last) {
      first = last = ch;
    } else if(ch == last+1) {
      last = ch;
    } else {
      FLUSHRANGE();
      first = last = ch;
    }
  }
  if(first <= last) {
    FLUSHRANGE();
  }
  result += _T("]");
  return result;
}


class StdAsciiFormater : public CharacterFormater {
public:
  String formatChar(_TUCHAR ch) {
    return format(_istprint(ch) ? _T("%c") : _T("\\x%02x"), ch);
  };
};

class ExtendedAsciiFormater : public CharacterFormater {
public:
  String formatChar(_TUCHAR ch) {
    return format((ch < 256) ? _T("%c") : _T("\\x%04x"), ch);
  };
};

class EscapedAsciiFormater : public CharacterFormater {
private:
  const unsigned int m_maxPrintable;
  const bool         m_useHex;
public:
  String formatChar(_TUCHAR ch);
  EscapedAsciiFormater(unsigned int maxPrintable, bool useHex) : m_maxPrintable(maxPrintable), m_useHex(useHex) {
  }
};

// Returns a String that represents c. This will be the character itself for normal characters,
// and an escape sequence (\n, \t, \x00, ...), for most others. A ' is represented as \'.
// If "useHex" is true, then \xDD escape sequences are used. Otherwise, octal sequences (\DDD) are used.
String EscapedAsciiFormater::formatChar(_TUCHAR ch) {
  TCHAR result[10];
  ch &= 0xff;
  if((_T(' ') < ch) && (ch <= m_maxPrintable) && (ch != _T('\'')) && (ch != _T('\\'))) {
    result[0] = ch;
    result[1] = 0;
  } else {
    result[0] = _T('\\');
    result[2] = 0 ;

    switch(ch) {
    case _T('\\'): result[1] = _T('\\'); break;
    case _T('\''): result[1] = _T('\''); break;
    case _T('\b'): result[1] = _T('b' ); break;
    case _T('\t'): result[1] = _T('t' ); break;
    case _T('\f'): result[1] = _T('f' ); break;
    case _T('\r'): result[1] = _T('r' ); break;
    case _T('\n'): result[1] = _T('n' ); break;
    case _T(' ' ): result[1] = _T('s' ); break;
    default      : _stprintf(&result[1], m_useHex ? _T("x%02x") : _T("%03o"), ch);
                   break;
    }
  }
  return result;
}

static StdAsciiFormater      _stdAsciiFormater;
static ExtendedAsciiFormater _extendedAsciiFormater;
static EscapedAsciiFormater  _octalEscapedAsciiFormater(      127, false);
static EscapedAsciiFormater  _hexEscapedAsciiFormater(        127, true);
static EscapedAsciiFormater  _hexEscapedExtendedAsciiFormater(255, true);

CharacterFormater *CharacterFormater::stdFormater                     = &_stdAsciiFormater;
CharacterFormater *CharacterFormater::extendedAsciiFormater           = &_extendedAsciiFormater;
CharacterFormater *CharacterFormater::octalEscapedAsciiFormater       = &_octalEscapedAsciiFormater;
CharacterFormater *CharacterFormater::hexEscapedAsciiFormater         = &_hexEscapedAsciiFormater;
CharacterFormater *CharacterFormater::hexEscapedExtendedAsciiFormater = &_hexEscapedExtendedAsciiFormater;
