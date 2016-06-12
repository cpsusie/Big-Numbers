#include "pch.h"
#include "ByteMemoryStream.h"
#include <MyAssert.h>

ByteMemoryOutputStream::ByteMemoryOutputStream(BYTE *dst) {
  assert(dst != NULL);
  m_p        = dst;
  m_pos      = 0;
  m_dstArray = NULL;
}

ByteMemoryOutputStream::ByteMemoryOutputStream(ByteArray &dst) {
  m_p        = NULL;
  m_pos      = 0;
  m_dstArray = &dst;
}

void ByteMemoryOutputStream::putBytes(const BYTE *src, size_t n) {
  if(m_p) {
    memcpy(m_p+m_pos, src, n);
    m_pos += n;
  } else {
    m_dstArray->append(src,n);
  }
}

void ByteMemoryOutputStream::putByte(BYTE b) {
  if(m_p) {
    m_p[m_pos++] = b;
  } else {
    m_dstArray->append(&b,1);
  }
}
