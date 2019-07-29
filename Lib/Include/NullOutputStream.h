#pragma once

#include "ByteStream.h"

class NullOutputStream : public ByteOutputStream {
private:
  NullOutputStream(const NullOutputStream &src);            // Not defined. Class not cloneable
  NullOutputStream &operator=(const NullOutputStream &src); // Not defined. Class not cloneable

public:
  NullOutputStream() {
  }

  void putBytes(const BYTE *src, UINT n) {
    /* do nothing */
  }

  void putByte(BYTE b) {
    /* do nothing */
  }
};
