#include "pch.h"
#include <StrStream.h>

using namespace std;
using namespace OStreamHelper;

String toString(char ch, StreamSize width, FormatFlags flags) {
  return (TowstringStream(width, flags) << ch).str().c_str();
}

String toString(short n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(USHORT n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(int n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(UINT n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(long n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(ULONG n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(INT64 n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(UINT64 n, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << n).str().c_str();
}

String toString(float x, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << x).str().c_str();
}

String toString(double x, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << x).str().c_str();
}
