#include "pch.h"
#include "BigRealStream.h"

String toString(const BigReal &x, StreamSize precision, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  TowstringStream stream(precision, width, flags);
  return putBigReal(stream, x, separatorChar).str().c_str();
}
