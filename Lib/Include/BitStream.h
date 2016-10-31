#pragma once

#include "ByteStream.h"

class BitOutputStream : public ByteOutputStream {
private:
  ByteOutputStream &m_dst;
  unsigned char     m_mask;
  signed int        m_rack;
  void flush();
public:
  BitOutputStream(ByteOutputStream &dst);
  ~BitOutputStream();
  BitOutputStream(const BitOutputStream &src);            // not defined. BitOutputStream not cloneable
  BitOutputStream &operator=(const BitOutputStream &src); // not defined. BitOutputStream not cloneable
  void putBits(unsigned long code, int count);
  void putBit(int bit);
  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b) {
    putBits(b, 8);
  }
};

class BitInputStream : public ByteInputStream {
private:
  ByteInputStream &m_src;
  unsigned char    m_mask;
  signed int       m_rack;
public:
  BitInputStream(ByteInputStream &src);
  BitInputStream(const BitInputStream &src);              // not defined. BitInputStream not cloneable
  BitInputStream &operator=(const BitInputStream &src);   // not defined. BitInputStream not cloneable
  int getBit();                                           // throws Exception if not enough bytes
  unsigned long getBits(int count);                       // do
  intptr_t getBytes(BYTE *dst, size_t n);                 // do
  int getByte();                                          // do
};

