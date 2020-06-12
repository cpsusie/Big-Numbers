#include "pch.h"
#include <BitStream.h>

BitOutputStream::BitOutputStream(ByteOutputStream &dst) : m_dst(dst) {
  m_rack = 0;
  m_mask = 0x80;
}

BitOutputStream::~BitOutputStream() {
  if(m_mask != 0x80) flush();
}

void BitOutputStream::flush() {
  m_dst.putByte(m_rack);
  m_rack = 0;
  m_mask = 0x80;
}

void BitOutputStream::putBits(ULONG code, int count) {
  if(count == 0) return;
#if defined(_DEBUG)
  if(count > 32) {
    throwInvalidArgumentException(__TFUNCTION__, _T("count=%d. Max=32"), count);
  }
#endif

  for(ULONG rmask = 1L << (count-1); rmask; rmask >>= 1) {
    if(rmask & code) m_rack |= m_mask;
    if((m_mask >>= 1) == 0) flush();
  }
}

void BitOutputStream::putBytes(const BYTE *src, size_t n) {
  while(n--) {
    putBits(*(src++), 8);
  }
}
