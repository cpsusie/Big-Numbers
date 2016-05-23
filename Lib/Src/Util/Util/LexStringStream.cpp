#include "pch.h"
#include <Scanner.h>

bool LexStringStream::open(const String &str) {
  m_string = str;
  m_pos    = 0;
  return true;
}

int LexStringStream::getChars(_TUCHAR *dst, unsigned int n) {
  const unsigned int rest = m_string.length() - m_pos;
  if(n > rest) {
    n = rest;
  }
  MEMCPY(dst, m_string.cstr()+m_pos, n);
  m_pos += n;
  return n;
}
