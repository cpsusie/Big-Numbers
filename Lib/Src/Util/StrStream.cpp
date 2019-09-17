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

String &StrStream::formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision) { // static
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
    result += _T('0');
    if(precision == 0) {
      precision = 6;
    }
    addDecimalPoint(result);
    addZeroes(result, (size_t)precision);
    addExponentChar(result, flags);
    result += _T("+0");
    break;
  }
  return result;
}

const TCHAR *StrStream::s_infStr  = _T("inf"      );
const TCHAR *StrStream::s_qNaNStr = _T("nan(ind)" );
const TCHAR *StrStream::s_sNaNStr = _T("nan(snan)");

TCHAR *StrStream::formatpinf(TCHAR *dst, bool uppercase) { // static
  _tcscpy(dst, s_infStr);
  return uppercase ? _tcsupr(dst) : dst;
}

TCHAR *StrStream::formatninf(TCHAR *dst, bool uppercase) { // static
  *dst = '-';
  formatpinf(dst + 1, uppercase);
  return dst;
}

TCHAR *StrStream::formatqnan(TCHAR *dst, bool uppercase) { // static
  _tcscpy(dst, s_qNaNStr);
  return uppercase ? _tcsupr(dst) : dst;
}

TCHAR *StrStream::formatsnan(TCHAR *dst, bool uppercase) { // static
  _tcscpy(dst, s_sNaNStr);
  return uppercase ? _tcsupr(dst) : dst;
}

TCHAR *StrStream::formatUndefined(TCHAR *dst, int fpclass, bool uppercase, bool formatNinfAsPinf) {
  switch(fpclass) {
  case _FPCLASS_SNAN  :  // signaling NaN
    return formatsnan(dst, uppercase);
  case _FPCLASS_QNAN  :  // quiet NaN
    return formatqnan(dst, uppercase);
  case _FPCLASS_NINF  :  // negative infinity
    if(!formatNinfAsPinf) {
      return formatninf(dst, uppercase);
    } // else Continue case
  case _FPCLASS_PINF  :  // positive infinity
    return formatpinf(dst, uppercase);
  default             :
    throwInvalidArgumentException(__TFUNCTION__, _T("fpclass=%08X"), fpclass);
    break;
  }
  return dst;
}

StrStream &StrStream::appendFill(size_t count) {
  insert(length(), count, fill());
  return *this;
}

StrStream &StrStream::formatFilledField(const String &prefix, const String &str, int flags) {
  if(flags == -1) flags = this->flags();
  const intptr_t fillCount = (intptr_t)width() - (intptr_t)str.length() - (intptr_t)prefix.length();
  if(fillCount <= 0) {
    append(prefix).append(str);
  } else {
    switch(flags & ios::adjustfield) {
    case ios::left    :
      append(prefix).append(str).appendFill(fillCount);
      break;
    case ios::internal:
      append(prefix).appendFill(fillCount).append(str);
      break;
    case ios::right   : // every other combination is the same as right-adjustment
    default           :
      appendFill(fillCount).append(prefix).append(str);
      break;
    }
  }
  return *this;
}

StrStream &StrStream::formatFilledNumericField(const String &str, bool negative, int flags) {
  if(flags == -1) flags = this->flags();
  String prefixStr;
  if(negative) {
    prefixStr = _T("-");
  } else if((flags & ios::showpos) && (radix(flags) == 10)) {
    prefixStr = _T("+");
  }
  if(flags & ios::showbase) {
    switch(flags & ios::basefield) {
    case ios::oct:
      return formatFilledField(prefixStr, _T("0") + str, flags);
    case ios::hex:
      addHexPrefix(prefixStr, flags);
      break;
    }
  }
  return formatFilledField(prefixStr, str, flags);
}

StrStream &StrStream::formatFilledFloatField(const String &str, bool negative, int flags) {
  if(flags == -1) flags = this->flags();
  String prefixStr;
  if(negative) {
    prefixStr = _T("-");
  } else if(flags & ios::showpos) {
    prefixStr = _T("+");
  }
  if((flags & ios::floatfield) == ios::hexfloat) {
    addHexPrefix(prefixStr, flags);
  }
  return formatFilledField(prefixStr, str, flags);
}
