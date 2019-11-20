#include "pch.h"
#include "BigRealStream.h"

String toString(const BigRational &x, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  TowstringStream stream(width, flags);
  return putBigRational(stream, x, separatorChar).str().c_str();
}
