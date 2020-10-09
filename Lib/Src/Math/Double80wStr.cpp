#include "pch.h"
#include "Double80Str.h"

using namespace Double80Str;

Double80 _wcstod80_l(const wchar_t *s, wchar_t **end, _locale_t locale) {
  return parseDouble80<wchar_t>(s, end, locale);
}

wchar_t *d80tow(wchar_t *dst, Double80 x) {
  return d80tostr(dst, x);
}
