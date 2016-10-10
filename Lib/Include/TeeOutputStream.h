#pragma once

#include "ByteStream.h"

class TeeOutputStream : public ByteOutputStream {
private:
  ByteOutputStream &m_s1, &m_s2;
public:
  TeeOutputStream(ByteOutputStream &s1, ByteOutputStream &s2) : m_s1(s1), m_s2(s2) {
  }

  TeeOutputStream(const TeeOutputStream &src);            // not defined. TeeOutputStream not cloneable
  TeeOutputStream &operator=(const TeeOutputStream &src); // not defined. TeeOutputStream not cloneable

  void putBytes(const BYTE *src, UINT n);
  void putByte(BYTE b);
};
