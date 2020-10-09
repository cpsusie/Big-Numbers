#include "pch.h"
#include "Double80Str.h"

using namespace Double80Str;

Double80 _strtod80_l(const char *s, char **end, _locale_t locale) {
  return parseDouble80<UCHAR>((UCHAR*)s, (UCHAR**)end, locale);
}

char *d80toa(char *dst, Double80 x) {
  return d80tostr(dst, x);
}
