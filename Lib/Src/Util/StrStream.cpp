#include "pch.h"
#include <String.h>

#ifdef UNICODE
#include <comdef.h>
#include <atlconv.h>
#endif

#include <StrStream.h>

#ifdef UNICODE
//StrStream::StrStream(tostream &stream) : StreamParameters(stream) {//
//}

StrStream &StrStream::append(const char *str) {
  USES_CONVERSION;
  return append(A2W(str));
}

StrStream &StrStream::operator<<(const char *str) {
  return append(str);
}

StrStream &StrStream::operator<<(char ch) {
  const char tmp[2] = { ch, 0 };
  return append(tmp);
}
#endif

TCHAR StrStream::unputc() {
  TCHAR ch = 0;
  const size_t l = length();
  if(l > 0) {
    ch = (*this)[l-1];
    remove(l-1);
  }
  return ch;
}

#define addDecimalPoint(s) { s += _T("."); }
#define addExponentChar(s) { s += ((flags & std::ios::uppercase) ? _T("E") : _T("e")); }

void StrStream::formatZero(String &result, streamsize precision, long flags, streamsize maxPrecision) { // static
  if((flags & (std::ios::scientific|std::ios::fixed)) == std::ios::scientific) {
    result += _T("0");
    if((flags & std::ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += spaceString((maxPrecision<=0) ? precision : min(precision,maxPrecision),_T('0'));
      } 
    }
    addExponentChar(result);
    result += _T("+00"); 
  } else if((flags & (std::ios::scientific|std::ios::fixed)) == std::ios::fixed) {
    result += _T("0");
    if((flags & std::ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += spaceString(precision,_T('0'));
      }
    }
  } else { // neither scientific nor fixed format is specified
    result += _T("0");
    if(flags & std::ios::showpoint) {
      addDecimalPoint(result);
      precision = max(precision,1);
      result += spaceString(precision,_T('0'));
    }
  }
}
