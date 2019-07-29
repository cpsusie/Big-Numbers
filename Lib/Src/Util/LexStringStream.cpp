#include "pch.h"
#include <Scanner.h>

bool LexStringStream::open(const String &str) {
  m_string = str;
  m_pos    = 0;
  return true;
}

intptr_t LexStringStream::getChars(_TUCHAR *dst, size_t n) {
  const intptr_t rest = m_string.length() - m_pos;
  if((intptr_t)n > rest) {
    n = rest;
  }
  TMEMCPY(dst, m_string.cstr()+m_pos, n);
  m_pos += n;
  return n;
}
