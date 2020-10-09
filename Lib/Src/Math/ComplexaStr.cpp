#include "pch.h"
#include "ComplexStr.h"

using namespace ComplexStr;

Complex strtoc(const char *s, char **end) {
  return parseComplex(s, end, strtor);
}

char *ctoa(char *dst, const Complex &c) {
  return complextostr(dst, c, rtoa);
}
