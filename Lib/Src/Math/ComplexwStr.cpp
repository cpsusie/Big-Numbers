#include "pch.h"
#include "ComplexStr.h"

using namespace ComplexStr;

Complex wcstoc(const wchar_t *s, wchar_t **end) {
  return parseComplex(s, end, wcstor);
}

wchar_t *ctow(wchar_t *dst, const Complex &c) {
  return complextostr(dst, c, rtow);
}
