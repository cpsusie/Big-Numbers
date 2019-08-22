#pragma once

#include "StreamParameters.h"

// Used instead of standardclass strstream, which is slow!!! (at least in windows)
class StrStream : public StreamParameters, public String {
public:
  static constexpr TCHAR s_decimalPointChar = _T('.');
  static const     TCHAR *s_infStr;
  static const     TCHAR *s_qNaNStr;
  static const     TCHAR *s_sNaNStr;

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

  // helper functions when formating various numbertypes
  static inline void addDecimalPoint(String &s) {
    s += s_decimalPointChar;
  }
  static inline void addExponentChar(String &s, FormatFlags flags) {
    s += ((flags & std::ios::uppercase) ? _T('E') : _T('e'));
  }
  static inline void addHexExponentChar(String &s, FormatFlags flags) {
    s += ((flags & std::ios::uppercase) ? _T('P') : _T('p'));
  }
  static inline void addZeroes(String &s, size_t count) {
    s.insert(s.length(), count, '0');
  }
  static void removeTralingZeroDigits(String &s) {
    while(s.last() == '0') s.removeLast();
    if(s.last() == s_decimalPointChar) s.removeLast();
  }

  static inline void addHexPrefix(String &s, FormatFlags flags) {
    s += ((flags & std::ios::uppercase) ? _T("0X") : _T("0x"));
  }

  // append str, left or right aligned, width spaces added/inserted if getWidth() > str.length()
  // return *this
  StrStream &appendFilledField(const String &str, FormatFlags flags);

  static void formatZero(String &result, StreamSize precision, FormatFlags flags, StreamSize maxPrecision = 0);
  static void formatqnan(String &result, bool uppercase = false);
  static void formatsnan(String &result, bool uppercase = false);
  static void formatpinf(String &result, bool uppercase = false);
  static void formatninf(String &result, bool uppercase = false);

  // Assume !isfinite(x)
  template<class T> static String formatUndefined(const T &x, bool uppercase=false) {
    String result;
    if(isnan(x)) {
      if(_fpclass(x) == _FPCLASS_SNAN) {
        formatsnan(result, uppercase);
      } else {
        formatqnan(result, uppercase);
      }
    } else if(isPInfinity(x)) {
      formatpinf(result, uppercase);
    } else if(isNInfinity(x)) {
      formatninf(result, uppercase);
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
