#include "pch.h"
#include <BitStream.h>

BitOutputStream::BitOutputStream(ByteOutputStream &dst) : m_dst(dst) {
  m_rack = 0;
  m_mask = 0x80;
}

BitOutputStream::~BitOutputStream() {
  if(m_mask != 0x80) {
    flush();
  }
}

void BitOutputStream::flush() {
  m_dst.putByte(m_rack);
  m_rack = 0;
  m_mask = 0x80;
}

void BitOutputStream::putBit(int bit) {
  if(bit) {
    m_rack |= m_mask;
  }
  if((m_mask >>= 1) == 0) {
    flush();
  }
}

void BitOutputStream::putBits(unsigned long code, int count) {
  if(count == 0) {
    return;
  }
#ifdef _DEBUG
  if(count > 32) {
    throwInvalidArgumentException(__TFUNCTION__, _T("count=%d. Max=32"), count);
  }
#endif

  for(unsigned long rmask = 1L << (count-1); rmask; rmask >>= 1) {
    if(rmask & code) {
      m_rack |= m_mask;
    }
    if((m_mask >>= 1) == 0) {
      flush();
    }
  }
}

void BitOutputStream::putBytes(const BYTE *src, UINT n) {
  while(n--) {
    putBits(*(src++), 8);
  }
}
