#include "pch.h"
#include "ByteMemoryStream.h"


ByteMemoryInputStream::ByteMemoryInputStream(const BYTE *src) {
  assert(src != NULL);
  m_p        = src;
  m_size     = 0xffffffff;
  m_pos      = 0;
}

ByteMemoryInputStream::ByteMemoryInputStream(const ByteArray &src) {
  m_p        = src.getData();
  m_size     = src.size();
  m_pos      = 0;
}

intptr_t ByteMemoryInputStream::getBytes(BYTE *dst, size_t n) {
  const size_t rest = m_size - m_pos;
  n = min(rest, n);
  memcpy(dst, m_p+m_pos, n);
  m_pos += n;
  return n;
}

int ByteMemoryInputStream::getByte() {
  return (m_pos == m_size) ? EOF : m_p[m_pos++];
}

