#include "pch.h"
#include <Math/BigRational.h>
#include "BigRealStream.h"

String toString(const BigRational &x, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  tostrstream stream;
  stream.width(width);
  stream.flags(flags);
  return putBigRational(stream, x, separatorChar).str().c_str();
}
