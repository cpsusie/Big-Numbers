#include "pch.h"

String &String::insert(size_t pos, TCHAR ch) {
  if(pos <= m_len) {
    if(m_capacity < m_len + 2) {
      m_capacity = (m_len + 2) * 2;
      TCHAR *tmp = new TCHAR[m_capacity];
      TMEMCPY(tmp, m_buf, pos);
      tmp[pos] = ch;
      TMEMCPY(tmp + pos + 1, m_buf + pos, m_len++ - pos + 1);
      delete[] m_buf;
      m_buf = tmp;
    } else {
      TMEMMOVE(m_buf + pos + 1, m_buf + pos, m_len++ - pos + 1); // remember '\0'
      m_buf[pos] = ch;
    }
  }
  return *this;
}

String &String::insert(size_t pos, size_t count, TCHAR ch) {
  if((pos <= m_len) && (count > 0)) {
    const size_t newLen    = m_len + count;
    const size_t moveCount = m_len - pos;
    if(m_capacity < newLen+1) {
      TCHAR *tmp = new TCHAR[m_capacity = (newLen + 1) * 2];
      if(pos) TMEMCPY(tmp, m_buf, pos);
      TMEMSET(tmp + pos, ch, count);
      if(moveCount) TMEMCPY(tmp + pos + count, m_buf + pos, moveCount);
      delete[] m_buf;
      m_buf = tmp;
    } else {
      if(moveCount) TMEMMOVE(m_buf + pos + count, m_buf + pos, moveCount);
      TMEMSET(m_buf + pos, ch, count);
    }
    m_buf[m_len = newLen] = 0;
  }
  return *this;
}

String &String::insert(size_t pos, const String &s) {
  if((pos <= m_len) && (s.m_len > 0)) {
    if(&s == this) { // Very rare case
      String copy(s);
      return insert(pos, copy);
    }
    const size_t newLen    = m_len + s.m_len;
    const size_t moveCount = m_len - pos;
    if(m_capacity < newLen + 1) {
      TCHAR *tmp = new TCHAR[m_capacity = (newLen + 1) * 2];
      if(pos) TMEMCPY(tmp, m_buf, pos);
      TMEMCPY(tmp + pos, s.m_buf, s.m_len);
      if(moveCount) TMEMCPY(tmp + pos + s.m_len, m_buf + pos, moveCount);
      delete[] m_buf;
      m_buf = tmp;
    } else {
      if(moveCount) TMEMMOVE(m_buf + pos + s.m_len, m_buf + pos, moveCount);
      TMEMCPY(m_buf + pos, s.m_buf, s.m_len);
    }
    m_buf[m_len = newLen] = 0;
  }
  return *this;
}
