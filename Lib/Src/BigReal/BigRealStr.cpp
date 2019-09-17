#include "pch.h"
#include "BigRealStream.h"

String toString(const BigReal &x, StreamSize precision, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  tostrstream stream;
  stream.width(width);
  stream.precision(precision);
  stream.flags(flags);
  return putBigReal(stream, x, separatorChar).str().c_str();
}
