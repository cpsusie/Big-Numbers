#include "pch.h"

tostream &operator<<(tostream &f, const String &str) {
  return f << str.m_buf;
}

tistream &operator>>(tistream &f, String &str) {
  TCHAR buf[4096];
  f >> buf;
  str = buf;
  return f;
}

