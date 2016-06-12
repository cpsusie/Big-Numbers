#include "pch.h"
#include <CountedByteStream.h>

CountedByteInputStream::CountedByteInputStream(ByteCounter &counter, ByteInputStream &src) : m_counter(counter), m_src(src) {
//  ByteCounter *bc = &m_counter;
}

intptr_t CountedByteInputStream::getBytes(BYTE *dst, size_t n) {
  size_t         total        = 0;
  const unsigned int maxChunkSize = m_counter.getMaxChunkSize();
  if(maxChunkSize == 0) {
    throwException("maxChunkSize == 0");
  }
  while(n > 0) {
    const intptr_t need = min(n, maxChunkSize);
    const intptr_t got  = m_src.getBytes(dst, need);
    m_counter.incrCount(got);
    total += got;
    dst   += got;
    n     -= got;
    if(got < need) {
      break;
    }
  }
  return total;
}

int CountedByteInputStream::getByte() {
  const int c = m_src.getByte();
  if(c != EOF) {
    m_counter.incrCount(1);
  }
  return c;
}
