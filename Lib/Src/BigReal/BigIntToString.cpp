#include "pch.h"
#include <Math/BigReal/BigInt.h>
#include "BigRealStream.h"

String toString(const BigInt &x, StreamSize width, FormatFlags flags, TCHAR separatorChar) {
  TowstringStream stream(width, flags);
  return putBigInt(stream, x, separatorChar).str().c_str();
}
