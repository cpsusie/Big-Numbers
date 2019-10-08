#include "pch.h"
#include "BigRealStream.h"

String toString(const FullFormatBigReal &x, StreamSize precision, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  if(precision > 0) {
    return ::toString((BigReal&)x, precision, width, flags, separatorChar);
  } else {
    TowstringStream stream(width, flags);
    return putFullFormatBigReal(stream, x, separatorChar).str().c_str();
  }
}
