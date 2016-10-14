#include "pch.h"
#include <BitStream.h>

BitInputStream::BitInputStream(ByteInputStream &src) : m_src(src) {
  m_rack = 0;
  m_mask = 0x80;
}

#define CHECKREAD                                             \
if(m_mask == 0x80) {                                          \
  if((m_rack = m_src.getByte()) == EOF) {                     \
    throwException("BitInputStream:Unexpected end of input"); \
  }                                                           \
}

int BitInputStream::getBit() {
  CHECKREAD
  const int value = m_rack & m_mask;
  if((m_mask >>= 1) == 0) {
    m_mask = 0x80;
  }
  return value ? 1 : 0;
}

unsigned long BitInputStream::getBits(int count) {
  if(count == 0) {
    return 0;
  }
#ifdef _DEBUG
  if(count > 32) {
    throwInvalidArgumentException(__TFUNCTION__, _T("count=%d. Max=32"), count);
  }
#endif

  unsigned long result = 0;
  for(unsigned long rmask = 1L << (count-1); rmask; rmask >>= 1) {
    CHECKREAD
    if(m_rack & m_mask) {
      result |= rmask;
    }
    if((m_mask >>= 1) == 0) {
      m_mask = 0x80;
    }
  }
  return result;
}

int BitInputStream::getBytes(BYTE *dst, UINT n) {
  for(int i = n; i--;) {
    *(dst++) = (BYTE)getBits(8);
  }
  return n;
}

int BitInputStream::getByte() {
  return getBits(8);
}
