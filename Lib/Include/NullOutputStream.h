#pragma once

#include "ByteStream.h"

class NullOutputStream : public ByteOutputStream {
public:
  NullOutputStream() {
  }

  NullOutputStream(const NullOutputStream &src);            // not defined. NullOutputStream not cloneable
  NullOutputStream &operator=(const NullOutputStream &src); // not defined. NullOutputStream not cloneable

  void putBytes(const BYTE *src, unsigned int n) {
    /* do nothing */
  }

  void putByte(BYTE b) {
    /* do nothing */
  }
};
