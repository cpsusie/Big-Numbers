#include "pch.h"

String &String::remove(size_t pos, size_t count) {
  if((pos < m_len) && (count > 0)) {
    size_t j = pos + count;
    if(j > m_len) {
      j = m_len;
      count = j - pos;
    }
    if(j < m_len) {
      TMEMMOVE(m_buf + pos, m_buf + j, m_len - j);
    }
    m_len -= count;
    m_buf[m_len] = 0;
  }
  return *this;
}

