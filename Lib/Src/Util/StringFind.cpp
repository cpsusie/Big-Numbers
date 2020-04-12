#include "pch.h"

intptr_t String::find(TCHAR ch, size_t from) const {
  if(from >= m_len) {
    return -1;
  }
  TCHAR *s = _tcschr(m_buf + from, ch);
  return s ? s - m_buf : -1;
}

intptr_t String::find(const String &str, size_t from) const {
  if(str.length() + from > m_len) {
    return -1;
  }
  TCHAR *s = _tcsstr(m_buf + from, str.m_buf);
  return s ? (s - m_buf) : -1;
}

intptr_t String::find(const TCHAR *str, size_t from) const {
  if(from > m_len) {
    return -1;
  }
  TCHAR *s = _tcsstr(m_buf + from, str);
  return s ? (s - m_buf) : -1;
}

intptr_t String::rfind(TCHAR ch) const {
  TCHAR *s = _tcsrchr(m_buf, ch);
  return s ? s - m_buf : -1;
}
