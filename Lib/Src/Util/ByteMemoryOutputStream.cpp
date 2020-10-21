#include "pch.h"
#include "ByteMemoryStream.h"

ByteMemoryOutputStream::ByteMemoryOutputStream(BYTE *dst) {
  assert(dst != nullptr);
  m_p        = dst;
  m_pos      = 0;
  m_dstArray = nullptr;
}

ByteMemoryOutputStream::ByteMemoryOutputStream(ByteArray &dst) {
  m_p        = nullptr;
  m_pos      = 0;
  m_dstArray = &dst;
}

void ByteMemoryOutputStream::putBytes(const BYTE *src, size_t n) {
  if(m_p) {
    memcpy(m_p+m_pos, src, n);
    m_pos += n;
  } else {
    m_dstArray->add(src,n);
  }
}

void ByteMemoryOutputStream::putByte(BYTE b) {
  if(m_p) {
    m_p[m_pos++] = b;
  } else {
    m_dstArray->add(&b,1);
  }
}
