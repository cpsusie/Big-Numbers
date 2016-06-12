#include "pch.h"
#include <String.h>

#ifdef UNICODE
#include <comdef.h>
#include <atlconv.h>
#endif

#include <StrStream.h>

StrStream::StrStream(int precision, int width, int flags) : StreamParameters(precision,width,flags) {
}

StrStream::StrStream(tostream &stream) : StreamParameters(stream) {
}

StrStream &StrStream::append(const TCHAR *s) {
  *this += s;
  return *this;
}

StrStream &StrStream::append(const String &s) {
  *this += s;
  return *this;
}

StrStream &StrStream::operator<<(const TCHAR *str) {
  return append(format(getStringFormat().cstr(), str));
}


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

StrStream &StrStream::operator<<(char        ch) {
  const char tmp[2] = { ch, 0 };
  return append(tmp);
}
#endif

StrStream &StrStream::operator<<(TCHAR ch) {
  return append(format(getCharFormat().cstr(), ch));
}

StrStream &StrStream::operator<<(const String &str) {
  return append(format(getStringFormat().cstr(), str.cstr()));
}

StrStream &StrStream::operator<<(int n) {
  return append(format(getIntFormat().cstr(), n));
}

StrStream &StrStream::operator<<(unsigned int n) {
  return append(format(getUIntFormat().cstr(), n));
}

StrStream &StrStream::operator<<(long n) {
  return append(format(getLongFormat().cstr(), n));
}

StrStream &StrStream::operator<<(unsigned long n) {
  return append(format(getULongFormat().cstr(), n));
}

StrStream &StrStream::operator<<(__int64 n) {
  return append(format(getInt64Format().cstr(), n));
}

StrStream &StrStream::operator<<(unsigned __int64 n) {
  return append(format(getUInt64Format().cstr(), n));
}

StrStream &StrStream::operator<<(float f) {
  return append(format(getFloatFormat().cstr(), f));
}

StrStream &StrStream::operator<<(double d) {
  return append(format(getDoubleFormat().cstr(), d));
}

StrStream &StrStream::operator<<(const StrStream &s) {
  return append(s.cstr());
}

StrStream &StrStream::operator<<(const StreamParameters &param) {
  ((StreamParameters&)(*this)) = param;
  return *this;
}

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

void StrStream::formatZero(String &result, int precision, long flags, int maxPrecision) { // static
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
