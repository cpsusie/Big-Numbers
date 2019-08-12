#include "pch.h"
#include <String.h>
#include <StrStream.h>

using namespace std;

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
#define addExponentChar(s) { s += ((flags & ios::uppercase) ? _T("E") : _T("e")); }

void StrStream::formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision) { // static
  if((flags & (ios::scientific|ios::fixed)) == ios::scientific) {
    result += _T("0");
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += spaceString((size_t)((maxPrecision<=0) ? precision : min(precision,maxPrecision)),_T('0'));
      }
    }
    addExponentChar(result);
    result += _T("+00");
  } else if((flags & (ios::scientific|ios::fixed)) == ios::fixed) {
    result += _T("0");
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += spaceString((size_t)precision,_T('0'));
      }
    }
  } else { // neither scientific nor fixed format is specified
    result += _T("0");
    if(flags & ios::showpoint) {
      addDecimalPoint(result);
      precision = max(precision,1);
      result += spaceString((size_t)precision,_T('0'));
    }
  }
}

void StrStream::formatqnan(String &result) { // static
  result += _T("-nan(ind)");
}

void StrStream::formatsnan(String &result) { // static
  result += _T("nan");
}

void StrStream::formatpinf(String &result) { // static
  result += _T("inf");
}

void StrStream::formatninf(String &result) { // static
  result += _T("-inf");
}
