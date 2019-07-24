#pragma once

#include "ByteStream.h"
#include "ByteArray.h"

class ByteMemoryInputStream : public ByteInputStream {
private:
  const BYTE *m_p;
  size_t      m_size;
  size_t      m_pos;
public:
  ByteMemoryInputStream(const BYTE *src);
  ByteMemoryInputStream(const ByteArray &src);

  ByteMemoryInputStream(const ByteMemoryInputStream &src);            // not defined. ByteMemoryInputStream not cloneable
  ByteMemoryInputStream &operator=(const ByteMemoryInputStream &src); // not defined. ByteMemoryInputStream not cloneable

  intptr_t getBytes(BYTE *dst, size_t n);
  int getByte();
};

class ByteMemoryOutputStream : public ByteOutputStream {
private:
  BYTE      *m_p;
  size_t     m_pos;
  ByteArray *m_dstArray;
public:
  ByteMemoryOutputStream(BYTE *dst);
  ByteMemoryOutputStream(ByteArray &dst);

  ByteMemoryOutputStream(const ByteMemoryOutputStream &src);            // not defined. ByteMemoryOutputStream not cloneable
  ByteMemoryOutputStream &operator=(const ByteMemoryOutputStream &src); // not defined. ByteMemoryOutputStream not cloneable

  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b);
};
