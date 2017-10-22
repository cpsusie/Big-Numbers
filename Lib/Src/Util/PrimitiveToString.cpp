#include "pch.h"
#include <StrStream.h>

String toString(char ch, int width, int flags) {
  StrStream stream(0,width,flags);
  stream << ch;
  return stream;
}

String toString(short n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(USHORT n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(int n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(UINT n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(long n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(ULONG n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(INT64 n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(UINT64 n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(float x, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return stream;
}

String toString(double x, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return stream;
}
