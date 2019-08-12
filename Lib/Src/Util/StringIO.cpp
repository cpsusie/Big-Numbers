#include "pch.h"

using namespace std;

ostream &operator<<(ostream &out, const String &str) {
  USES_ACONVERSION;
  return out << TSTR2ASTR(str.cstr());
}

istream &operator>>(istream &in, String &str) {
  char buf[4096];
  in >> buf;
  str = buf;
  return in;
}

wostream &operator<<(wostream &out, const String &str) {
  USES_WCONVERSION;
  return out << TSTR2WSTR(str.cstr());
}

wistream &operator>>(wistream &in, String &str) {
  wchar_t buf[4096];
  in >> buf;
  str = buf;
  return in;
}

