#pragma once

#include "ByteStream.h"

class ByteMemoryInputStream : public ByteInputStream {
private:
  const BYTE      *m_p;
  unsigned int     m_size;
  unsigned int     m_pos;
public:
  ByteMemoryInputStream(const BYTE *src);
  ByteMemoryInputStream(const ByteArray &src);

  ByteMemoryInputStream(const ByteMemoryInputStream &src);            // not defined. ByteMemoryInputStream not cloneable
  ByteMemoryInputStream &operator=(const ByteMemoryInputStream &src); // not defined. ByteMemoryInputStream not cloneable

  int getBytes(BYTE *dst, unsigned int n);
  int getByte();
};

class ByteMemoryOutputStream : public ByteOutputStream {
private:
  BYTE        *m_p;
  unsigned int m_pos;
  ByteArray   *m_dstArray;
public:
  ByteMemoryOutputStream(BYTE *dst);
  ByteMemoryOutputStream(ByteArray &dst);

  ByteMemoryOutputStream(const ByteMemoryOutputStream &src);            // not defined. ByteMemoryOutputStream not cloneable
  ByteMemoryOutputStream &operator=(const ByteMemoryOutputStream &src); // not defined. ByteMemoryOutputStream not cloneable

  void putBytes(const BYTE *src, unsigned int n);
  void putByte(BYTE b);
};
