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

void StrStream::formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision) { // static
  switch(flags & ios::floatfield) {
  case 0:
    result += _T('0');
    if(flags & ios::showpoint) {
      addDecimalPoint(result);
      precision = max(precision, 1);
      addZeroes(result, (size_t)precision);
    }
    break;
  case ios::scientific:
    result += _T('0');
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        addZeroes(result, (size_t)((maxPrecision<=0) ? precision : min(precision,maxPrecision)));
      }
    }
    addExponentChar(result, flags);
    result += _T("+00");
    break;
  case ios::fixed:
    result += _T('0');
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        addZeroes(result, (size_t)precision);
      }
    }
    break;
  case ios::hexfloat:
    addHexPrefix(result, flags);
    result += _T('0');
    if(precision == 0) {
      precision = 6;
    }
    addDecimalPoint(result);
    addZeroes(result, (size_t)precision);
    addHexExponentChar(result, flags);
    result += _T("+0");
    break;
  }
}

const TCHAR *StrStream::s_infStr  = _T("inf"      );
const TCHAR *StrStream::s_qNaNStr = _T("-nan(ind)");
const TCHAR *StrStream::s_sNaNStr = _T("nan"      );

void StrStream::formatpinf(String &result, bool uppercase) { // static
  result += uppercase ? toUpperCase(s_infStr) : s_infStr;
}

void StrStream::formatninf(String &result, bool uppercase) { // static
  result += _T("-");
  formatpinf(result, uppercase);
}

void StrStream::formatqnan(String &result, bool uppercase) { // static
  result += uppercase ? toUpperCase(s_qNaNStr) : s_qNaNStr;
}

void StrStream::formatsnan(String &result, bool uppercase) { // static
  result += uppercase ? toUpperCase(s_sNaNStr) : s_sNaNStr;
}

StrStream &StrStream::appendFilledField(const String &str, FormatFlags flags) {
  const intptr_t fillerLength = (intptr_t)getWidth() - (intptr_t)str.length();
  if(fillerLength <= 0) {
    append(str);
  } else if((flags & (ios::left | ios::right)) == ios::left) { // adjust left iff only ios::left is set
    append(str).append(spaceString(fillerLength));
  } else { // right align
    append(spaceString(fillerLength)).append(str);
  }
  return *this;
}
