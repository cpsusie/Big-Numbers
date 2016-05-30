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

String toString(unsigned short n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(int n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(unsigned int n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(long n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(unsigned long n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(__int64 n, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << n;
  return stream;
}

String toString(unsigned __int64 n, int precision, int width, int flags) {
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
