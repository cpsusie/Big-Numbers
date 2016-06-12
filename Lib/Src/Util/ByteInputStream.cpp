#include "pch.h"
#include <ByteStream.h>

void ByteInputStream::getBytesForced(BYTE *dst, size_t n) { // throws "unexpected end-of-stream Exception", if not all bytes read
  const intptr_t got = getBytes(dst,n);
  if(got != n) {
    throwException(_T("Unexpected end of ByteInputStream. Tried to read %lu bytes, got only %lu."), n, got);
  }
}
