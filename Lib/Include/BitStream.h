#pragma once

#include "ByteStream.h"

class BitOutputStream : public ByteOutputStream {
private:
  ByteOutputStream &m_dst;
  BYTE              m_mask;
  signed int        m_rack;
  void flush();
public:
  BitOutputStream(ByteOutputStream &dst);
  ~BitOutputStream();
  BitOutputStream(const BitOutputStream &src);            // not defined. BitOutputStream not cloneable
  BitOutputStream &operator=(const BitOutputStream &src); // not defined. BitOutputStream not cloneable
  void putBits(ULONG code, int count);
  inline void putBit(int bit) {
    if(bit) m_rack |= m_mask;
    if((m_mask >>= 1) == 0) flush();
  }
  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b) {
    putBits(b, 8);
  }
};

class BitInputStream : public ByteInputStream {
private:
  ByteInputStream &m_src;
  BYTE             m_mask;
  signed int       m_rack;
  inline void nextByte() {
    if((m_rack = m_src.getByte()) == EOF) {
      throwException(_T("BitInputStream:Unexpected end of input"));
    }
  }
public:
  inline BitInputStream(ByteInputStream &src) : m_src(src) {
    m_rack = 0;
    m_mask = 0x80;
  }

  BitInputStream(const BitInputStream &src);              // not defined. BitInputStream not cloneable
  BitInputStream &operator=(const BitInputStream &src);   // not defined. BitInputStream not cloneable

  inline int getBit() {                                   // throws Exception if not enough bytes
    if(m_mask == 0x80) nextByte();
    const int value = m_rack & m_mask;
    if((m_mask >>= 1) == 0) m_mask = 0x80;
    return value ? 1 : 0;
  }
  ULONG getBits(int count);                               // do
  intptr_t getBytes(BYTE *dst, size_t n);                 // do
  int getByte();                                          // do
};
