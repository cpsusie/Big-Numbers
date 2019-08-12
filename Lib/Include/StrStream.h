#pragma once

#include "StreamParameters.h"

// Used instead of standardclass strstream, which is slow!!! (at least in windows)
class StrStream : public StreamParameters, public String {
public:
  inline StrStream(StreamSize precision = 6, StreamSize width = 0, FormatFlags flags = 0) : StreamParameters(precision, width, flags) {
  }
  inline StrStream(const StreamParameters &param) : StreamParameters(param) {
  }
  inline StrStream(std::ostream &stream) : StreamParameters(stream) {
  }
  inline StrStream(std::wostream &stream) : StreamParameters(stream) {
  }

  inline void clear() {
    String::operator=(EMPTYSTRING);
  }

  inline TCHAR getLast() const {
    return last();
  }

  // append str, left or right aligned, width spaces added/inserted if getWidth() > str.length()
  // return *this
  StrStream &appendFilledField(const String &str, FormatFlags flags);

  static void formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision = 0);
  static void formatqnan(String &result);
  static void formatsnan(String &result);
  static void formatpinf(String &result);
  static void formatninf(String &result);

  // Assume !isfinite(x)
  template<class T> static String formatUndefined(const T &x) {
    String result;
    if(isnan(x)) {
      if(_fpclass(x) == _FPCLASS_SNAN) {
        formatsnan(result);
      } else {
        formatqnan(result);
      }
    } else if(isPInfinity(x)) {
      formatpinf(result);
    } else if(isNInfinity(x)) {
      formatninf(result);
    } else {
      return format(_T("%s:x not nan,pinf or ninf"), __TFUNCTION__);
    }
    return result;
  }

  TCHAR unputc();
  inline StrStream &append(const String &str) { // append str to stream without using any format-specifiers
    *this += str;
    return *this;
  }
  inline StrStream &append(const char *str) {
    *this += str;
    return *this;
  }
  inline StrStream &append(const wchar_t *str) {
    *this += str;
    return *this;
  }

  inline StrStream &operator<<(wchar_t ch) {
    return append(format(getCharFormat().cstr(), (TCHAR)ch));
  }
  inline StrStream &operator<<(char ch) {
    return append(format(getCharFormat().cstr(), (TCHAR)ch));
  }
  StrStream &operator<<(      BYTE ch) {
    return append(format(getCharFormat().cstr(), (_TUCHAR)ch));
  }
  inline StrStream &operator<<(const BYTE *str) {
    USES_ACONVERSION;
    return append(format(getStringFormat().cstr(), ASTR2TSTR((char*)str)));
  }
  inline StrStream &operator<<(const char *str) {
    USES_ACONVERSION;
    return append(format(getStringFormat().cstr(), ASTR2TSTR(str)));
  }
  inline StrStream &operator<<(const wchar_t *str) {
    USES_WCONVERSION;
    return append(format(getStringFormat().cstr(), WSTR2TSTR(str)));
  }
  inline StrStream &operator<<(const String &str) {
    return append(format(getStringFormat().cstr(), str.cstr()));
  }
  inline StrStream &operator<<(int n) {
    return append(format(getIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(UINT n) {
    return append(format(getUIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(long n) {
    return append(format(getLongFormat().cstr(), n));
  }
  inline StrStream &operator<<(ULONG n) {
    return append(format(getULongFormat().cstr(), n));
  }
  inline StrStream &operator<<(INT64 n) {
    return append(format(getInt64Format().cstr(), n));
  }
  inline StrStream &operator<<(UINT64 n) {
    return append(format(getUInt64Format().cstr(), n));
  }
  inline StrStream &operator<<(float f) {
    return append(format(getFloatFormat().cstr(), f));
  }
  inline StrStream &operator<<(double d) {
    return append(format(getDoubleFormat().cstr(), d));
  }
  inline StrStream &operator<<(const StrStream &s) {
    return append(s.cstr());
  }
  inline StrStream &operator<<(const StreamParameters &param) {
    ((StreamParameters&)(*this)) = param;
    return *this;
  }
};
