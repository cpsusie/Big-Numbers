#include "pch.h"
#include <StrStream.h>
#include <Math/Int128.h>

String toString(const _int128  &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(const _uint128 &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}
