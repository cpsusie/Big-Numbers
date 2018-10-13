#include "pch.h"
#include <String.h>
#include <StrStream.h>

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

void StrStream::formatnan(String &result) { // static
  result += _T("-nan(ind)");
}

void StrStream::formatpinf(String &result) { // static
  result += _T("inf");
}

void StrStream::formatninf(String &result) { // static
  result += _T("-inf");
}
