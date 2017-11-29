#include "pch.h"

std::ostream &operator<<(std::ostream &out, const String &str) {
  USES_ACONVERSION;
  return out << TSTR2ASTR(str.cstr());
}

std::istream &operator>>(std::istream &in, String &str) {
  char buf[4096];
  in >> buf;
  str = buf;
  return in;
}

std::wostream &operator<<(std::wostream &out, const String &str) {
  USES_WCONVERSION;
  return out << TSTR2WSTR(str.cstr());
}

std::wistream &operator>>(std::wistream &in, String &str) {
  wchar_t buf[4096];
  in >> buf;
  str = buf;
  return in;
}

