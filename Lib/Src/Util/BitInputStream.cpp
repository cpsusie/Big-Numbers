#include "pch.h"
#include <BitStream.h>

ULONG BitInputStream::getBits(int count) {
  if(count == 0) return 0;
#if defined(_DEBUG)
  if(count > 32) {
    throwInvalidArgumentException(__TFUNCTION__, _T("count=%d. Max=32"), count);
  }
#endif

  ULONG result = 0;
  for(ULONG rmask = 1L << (count-1); rmask; rmask >>= 1) {
    if(m_mask == 0x80) nextByte();
    if(m_rack & m_mask) result |= rmask;
    if((m_mask >>= 1) == 0) m_mask = 0x80;
  }
  return result;
}

intptr_t BitInputStream::getBytes(BYTE *dst, size_t n) {
  for(size_t i = n; i--;) {
    *(dst++) = (BYTE)getBits(8);
  }
  return n;
}

int BitInputStream::getByte() {
  return getBits(8);
}
