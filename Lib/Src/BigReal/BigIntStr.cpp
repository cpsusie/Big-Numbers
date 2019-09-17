#include "pch.h"
#include <Math/BigInt.h>
#include "BigRealStream.h"

String toString(const BigInt &x, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  tostrstream stream;
  stream.width(width);
  stream.flags(flags);
  return putBigInt(stream, x, separatorChar).str().c_str();
}
