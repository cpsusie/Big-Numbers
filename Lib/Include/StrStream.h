#pragma once

#include "MyString.h"
#include "StreamParameters.h"

class StrStream : public StreamParameters, public String { // used instead of standardclass strstream, which is slow!!! (at least in windows)
public:
  inline StrStream(streamsize precision = 6, streamsize width = 0, int flags = 0) : StreamParameters(precision, width, flags) {
  }
  inline StrStream(const StreamParameters &param) : StreamParameters(param) {
  }
  inline StrStream(ostream &stream) : StreamParameters(stream) {
  }
  inline StrStream(wostream &stream) : StreamParameters(stream) {
  }

  inline void clear() {
    String::operator=(_T(""));
  }

  inline TCHAR getLast() const {
    return last();
  }

  static void formatZero(String &result, streamsize precision, long flags, streamsize maxPrecision = 0);

  TCHAR unputc();
  inline StrStream &append(const String &str) { // append str to stream without using any format-specifiers
    *this += str;
    return *this;
  }
  inline StrStream &append(const TCHAR *str) {
    *this += str;
    return *this;
  }
  inline StrStream &operator<<(TCHAR ch) {
    return append(format(getCharFormat().cstr(), ch));
  }
  inline StrStream &operator<<(unsigned char ch);
  StrStream &operator<<(const unsigned char *str);
  inline StrStream &operator<<(const TCHAR *str) {
    return append(format(getStringFormat().cstr(), str));
  }
  inline StrStream &operator<<(const String &str) {
    return append(format(getStringFormat().cstr(), str.cstr()));
  }
  inline StrStream &operator<<(int n) {
    return append(format(getIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(unsigned int n) {
    return append(format(getUIntFormat().cstr(), n));
  }
  inline StrStream &operator<<(long n) {
    return append(format(getLongFormat().cstr(), n));
  }
  inline StrStream &operator<<(unsigned long n) {
    return append(format(getULongFormat().cstr(), n));
  }
  inline StrStream &operator<<(__int64 n) {
    return append(format(getInt64Format().cstr(), n));
  }
  inline StrStream &operator<<(unsigned __int64 n) {
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

#ifdef UNICODE
  StrStream &append(    const char *str);
  StrStream &operator<<(const char *str);
  StrStream &operator<<(char        ch);
#endif
};
