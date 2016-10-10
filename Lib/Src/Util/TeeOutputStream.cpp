#include "pch.h"
#include <TeeOutputStream.h>

void TeeOutputStream::putBytes(const BYTE *src, UINT n) {
  m_s1.putBytes(src, n);
  m_s2.putBytes(src, n);
}

void TeeOutputStream::putByte(BYTE b) {
  m_s1.putByte(b);
  m_s2.putByte(b);
}
