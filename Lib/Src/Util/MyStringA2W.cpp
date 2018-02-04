#include "pch.h"

String::String(const char *s) {
  USES_ACONVERSION;
  m_buf = newCharBuffer(A2TNULL(s), m_len, m_capacity);
}

String &String::operator=(const char *s) {
  USES_ACONVERSION;
  const TCHAR *ms     = A2TNULL(s);
  __assume(ms);
  const size_t length = _tcsclen(ms);
  if(length < m_capacity && length + 100 > m_capacity) {
    _tcscpy(m_buf, ms);
    m_len = length;
  } else {
    TCHAR *old = m_buf;
    m_buf      = newCharBuffer(ms, m_len, m_capacity);
    delete[] old;
  }
  return *this;
}

String &String::operator+=(const char *rhs) {
  USES_ACONVERSION;
  const TCHAR *mrhs   = A2TNULL(rhs);
  __assume(mrhs);
  const size_t length = _tcsclen(mrhs);
  if(length == 0) {
    return *this;
  }
  const size_t newLength = m_len + length;
  if(m_capacity < newLength + 1) {
    TCHAR *old = m_buf;
    m_buf = new TCHAR[m_capacity = (newLength + 1) * 3];
    MEMCPY(m_buf, old, m_len);
    _tcsncpy(m_buf + m_len, mrhs, length);
    m_buf[m_len = newLength] = '\0';
    delete[] old; // Dont delete old before now, rhs and old might overlap
  } else {
    _tcsncpy(m_buf + m_len, mrhs, length); // Dont use strcpy. rhs and m_buf might overlap, which would give an infinite loop
    m_buf[m_len = newLength] = '\0';
  }
  return *this;
}
