#include "stdafx.h"
#include <ctype.h>
#include <String.h>
#include <stdio.h>
#include "FileFormat.h"

static inline _TUCHAR toUpper(_TUCHAR ch) {
  return (_istascii(ch) && _istlower(ch)) ? _totupper(ch) : ch;
}

static inline bool isOctalDigit(_TUCHAR ch) {
  return (_T('0') <= ch) && (ch <= _T('7'));
}

/* Convert the hex digit represented by _T('c') to an int. _T('c') must be one of
 * the following characters: 0123456789abcdefABCDEF
 */
static inline int hexToInt(_TUCHAR ch) {
  return (_istdigit(ch) ? (ch-_T('0')): ((toUpper(ch))-_T('A'))+10) & 0xf;
}

/* Convert the hex digit represented by _T('c') to an int. _T('c') must be a
 * digit in the range _T('0')-_T('7').
 */
static inline int octalToInt(_TUCHAR ch) {
  return (ch-_T('0')) & 0x7;
}

/*------------------------------------------------------------*/

/* Map escape sequences into their equivalent symbols. Return the equivalent
 * ASCII character. s is advanced past the escape sequence. If no escape
 * sequence is present, the current character is returned and the String
 * is advanced by one. The following are recognized:
 *
 *  \b      Backspace
 *  \f      Formfeed
 *  \t      Tab
 *  \n      Newline
 *  \r      Carriage return
 *  \s      Space
 *  \e      ASCII ESC character (_T('\033'))
 *  \DDD    Number formed of 1-3 octal digits
 *  \xDD    Number formed of 1-3 hex digits
 *  \^C     C = any letter. Control code
 *  \uDDDD  Number formed by 4 hex digits. Only if UNICODE is defined
 */
static _TUCHAR escape(const _TUCHAR *&s) {
  UINT result;

  if(*s != _T('\\') ) {
    result = *(s++);
  } else {
    s++; // Skip the _T('\') 
    switch(toUpper(*s)) {
    case _T('\0'):
      result = _T('\\')  ;
      return result;
      break;
    case _T('B') :
      result = _T('\b')  ;
      break;
    case _T('F') :
      result = _T('\f')  ;
      break;
    case _T('N') :
      result = _T('\n')  ;
      break;
    case _T('R') :
      result = _T('\r')  ;
      break;
    case _T('S') :
      result = _T(' ')   ;
      break;
    case _T('T') :
      result = _T('\t')  ;
      break;
    case _T('E') :
      result = _T('\033');
      break;

    case _T('^') :
      s++;
      result = *s;
      if(result >= _T('@')) {
        result = toUpper(result) - _T('@') ; // _T('@') = 0x40
      }
      break;

    case _T('X') :
      result = 0; s++; // skip x
      if(_istxdigit(*s)) {
        result = hexToInt(*(s++));
      }
      if(_istxdigit(*s)) {
        result = result * 16 + hexToInt(*(s++));
      }
      s--;
      break;

#ifdef UNICODE
    case _T('U'):
      { result = 0; s++; // skip u
        int i;
        for(i = 0; i < 4; i++) {
          if(_istxdigit(*s)) result = result * 16 + hexToInt(*(s++)); else break;
        }
        if(i) {
          s--;
        } else { // No hex digits after \u was found. Just return '\\'
          s-=2;
          result = '\\';
        }
      }
      break;
#endif      
    default  :
      if(!isOctalDigit(*s)) {
        result = *s;
      } else {
        s++;
        result = octalToInt(*(s++));
        if(isOctalDigit(*s)                          ) result = result  * 8 + octalToInt(*(s++));
        if(isOctalDigit(*s) && (result <= _T('\037'))) result = result  * 8 + octalToInt(*(s++));
        s--;
      }
      break;
    }
    s++;
  }
  return result;
}

String convertEscape(const String &s) {
  TCHAR *tmp = new TCHAR[s.length() + 1];
  _TUCHAR *d = (_TUCHAR*)tmp;
  for(const _TUCHAR *t = (_TUCHAR*)s.cstr();*t;) {
    if(*t == _T('\\')) {
      *(d++) = escape(t);
    } else {
      *(d++) = *(t++);
    }
  }
  *d = 0;
  String result(tmp);
  delete[] tmp;
  return result;
}

String expandEscape(const String &s) {
  String result;
  for(const _TUCHAR *t = (_TUCHAR*)s.cstr(); *t; t++) {
    switch(*t) {
    case _T('\t')  : result += _T("\\t") ; break;
    case _T('\f')  : result += _T("\\f") ; break;
    case _T('\n')  : result += _T("\\n") ; break;
    case _T(' ' )  : result += _T("\\s") ; break;
    case _T('\r')  : result += _T("\\r") ; break;
    case _T('\033'): result += _T("\\e") ; break;
    case _T('\\')  : result += _T("\\\\"); break;
    default:
      if(*t < _T(' ')) {
        result += format(_T("\\^%c"), (*t) + _T('@'));
#ifdef UNICODE
      } else if(*t > 255) {
        result += format(_T("\\u%04x"), *t);
#endif
      } else if(*t > 126) {
        result += format(_T("\\x%x"), *t);
      } else {
        result += *t;
      }
      break;
    }
  }
  return result;
}
