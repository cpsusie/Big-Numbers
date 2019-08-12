#include "pch.h"
#include <StrStream.h>

String toString(char ch, StreamSize width, FormatFlags flags) {
  StrStream stream(0,width,flags);
  stream << ch;
  return stream;
}

String toString(short n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(USHORT n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(int n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(UINT n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(long n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(ULONG n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(INT64 n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(UINT64 n, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(float x, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return stream;
}

String toString(double x, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return stream;
}
