#pragma once

#include "MyUtil.h"

class ByteInputStream {
public:
  // Must return the actual number of bytes read.
  virtual intptr_t getBytes(BYTE *dst, size_t n) = 0;
  // Must return EOF when end-of-stream
  virtual int getByte() = 0;
  // Throws "unexpected end-of-stream Exception", if the requested number of bytes cannot be read
  void getBytesForced(BYTE *dst, size_t n);
  virtual ~ByteInputStream() {
  }
};

class ByteOutputStream {
public:
  virtual void putBytes(const BYTE *src, size_t n) = 0;
  virtual void putByte(BYTE b) = 0;
  virtual ~ByteOutputStream() {
  }
};

class ByteInputOutputStream : public ByteInputStream, public ByteOutputStream {
};

class ResetableByteInputStream : public ByteInputStream {
public:
  // Prepare to read bytes again
  virtual void reset() = 0;
};

class ResetableByteInputOutputStream : public ResetableByteInputStream, public ByteOutputStream {
};

