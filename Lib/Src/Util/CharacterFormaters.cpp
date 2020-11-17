#include "pch.h"

class StdAsciiFormater : public CharacterFormater {
public:
  String toString(const size_t &ch) override {
    return format(_istprint((_TUCHAR)ch) ? _T("%c") : _T("\\x%02x"), (_TUCHAR)ch);
  };
};

class ExtendedAsciiFormater : public CharacterFormater {
public:
  String toString(const size_t &ch) override {
    return format((ch < 256) ? _T("%c") : _T("\\x%04x"), (_TUCHAR)ch);
  };
};

class EscapedAsciiFormater : public CharacterFormater {
private:
  const UINT m_maxPrintable;
  const bool m_useHex;
public:
  String toString(const size_t &ch) override;
  EscapedAsciiFormater(UINT maxPrintable, bool useHex) : m_maxPrintable(maxPrintable), m_useHex(useHex) {
  }
};

// Returns a String that represents c. This will be the character itself for normal characters,
// and an escape sequence (\n, \t, \x00, ...), for most others. A ' is represented as \'.
// If "useHex" is true, then \xDD escape sequences are used. Otherwise, octal sequences (\DDD) are used.
String EscapedAsciiFormater::toString(const size_t &_ch) {
  TCHAR result[10];
  _TUCHAR ch = _ch & 0xff;
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

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static StdAsciiFormater      _stdAsciiFormater;
static ExtendedAsciiFormater _extendedAsciiFormater;
static EscapedAsciiFormater  _octalEscapedAsciiFormater(      127, false);
static EscapedAsciiFormater  _hexEscapedAsciiFormater(        127, true );
static EscapedAsciiFormater  _hexEscapedExtendedAsciiFormater(255, true );

CharacterFormater &CharacterFormater::stdAsciiFormater                = _stdAsciiFormater;
CharacterFormater &CharacterFormater::extendedAsciiFormater           = _extendedAsciiFormater;
CharacterFormater &CharacterFormater::octalEscapedAsciiFormater       = _octalEscapedAsciiFormater;
CharacterFormater &CharacterFormater::hexEscapedAsciiFormater         = _hexEscapedAsciiFormater;
CharacterFormater &CharacterFormater::hexEscapedExtendedAsciiFormater = _hexEscapedExtendedAsciiFormater;
