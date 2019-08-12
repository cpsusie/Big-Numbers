#include "pch.h"

String &String::insert(size_t pos, TCHAR ch) {
  if (pos <= m_len) {
    if (m_capacity < m_len + 2) {
      m_capacity = (m_len + 2) * 2;
      TCHAR *tmp = new TCHAR[m_capacity];
      TMEMCPY(tmp, m_buf, pos);
      tmp[pos] = ch;
      TMEMCPY(tmp + pos + 1, m_buf + pos, m_len++ - pos + 1);
      delete[] m_buf;
      m_buf = tmp;
    }
    else {
      TMEMMOVE(m_buf + pos + 1, m_buf + pos, m_len++ - pos + 1); // remember '\0'
      m_buf[pos] = ch;
    }
  }
  return *this;
}

String &String::insert(size_t pos, const String &s) {
  if ((pos <= m_len) && (s.m_len > 0)) {
    if (m_capacity < m_len + s.m_len + 1) {
      TCHAR *tmp = new TCHAR[m_capacity = (m_len + s.m_len + 1) * 2];
      if (pos) {
        TMEMCPY(tmp, m_buf, pos);
      }
      TMEMCPY(tmp + pos, s.m_buf, s.m_len);
      TMEMCPY(tmp + pos + s.m_len, m_buf + pos, m_len - pos + 1);
      m_len += s.m_len;
      delete[] m_buf;
      m_buf = tmp;
    }
    else {
      TMEMMOVE(m_buf + pos + s.m_len, m_buf + pos, m_len - pos + 1); // remember '\0'
      TMEMCPY(m_buf + pos, s.m_buf, s.m_len);
      m_len += s.m_len;
    }
  }
  return *this;
}
