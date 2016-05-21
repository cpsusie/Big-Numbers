#include "pch.h"
#include <CountedByteStream.h>

CountedByteOutputStream::CountedByteOutputStream(ByteCounter &counter, ByteOutputStream &dst) : m_counter(counter), m_dst(dst) {
}

void CountedByteOutputStream::putBytes(const BYTE *src, unsigned int n) {
  while(n > 0) {
    unsigned int m = min(n, 500000);
    m_dst.putBytes(src, m);
    m_counter.incrCount(m);
    n   -= m;
    src += m;
  }
}

void CountedByteOutputStream::putByte(BYTE b) {
  m_dst.putByte(b);
  m_counter.incrCount(1);
}
