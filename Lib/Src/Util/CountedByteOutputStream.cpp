#include "pch.h"
#include <CountedByteStream.h>

CountedByteOutputStream::CountedByteOutputStream(ByteCounter &counter, ByteOutputStream &dst) : m_counter(counter), m_dst(dst) {
}

void CountedByteOutputStream::putBytes(const BYTE *src, size_t n) {
  while(n > 0) {
    const size_t m = min(n, 500000);
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
