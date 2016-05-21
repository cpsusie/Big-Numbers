#pragma once

#include "MyUtil.h"

class ByteInputStream {
public:
  virtual int getBytes(BYTE *dst, unsigned int n) = 0; // Must return the actual number of bytes read.
  virtual int getByte() = 0;                           // Must return EOF when end-of-stream
  void getBytesForced(BYTE *dst, unsigned int n);      // Throws "unexpected end-of-stream Exception", if the requested number of bytes cannot be read
};

class ByteOutputStream {
public:
  virtual void putBytes(const BYTE *src, unsigned int n) = 0;
  virtual void putByte(BYTE b) = 0;
};

class ByteInputOutputStream : public ByteInputStream, public ByteOutputStream {
};

class ResetableByteInputStream : public ByteInputStream {
public:
  virtual void reset() = 0;                            // prepare to read bytes again
};

class ResetableByteInputOutputStream : public ResetableByteInputStream, public ByteOutputStream {
};

