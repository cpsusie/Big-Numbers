#include "pch.h"
#include <CountedByteStream.h>

CountedByteOutputStream::CountedByteOutputStream(ByteCounter &counter, ByteOutputStream &dst) : m_counter(counter), m_dst(dst) {
}

void CountedByteOutputStream::putBytes(const BYTE *src, size_t n) {
  const UINT maxChunkSize = m_counter.getMaxChunkSize();
  if(maxChunkSize == 0) {
    throwException(_T("maxChunkSize == 0"));
  }
  while(n > 0) {
    const size_t m = min(n, maxChunkSize);
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
