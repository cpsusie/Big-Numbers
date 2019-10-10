#include "pch.h"
#include <limits.h>
#include <StrStream.h>
#include <Math/Rational.h>

#define EATWHITE() { while(iswspace(*s)) s++; }

template<class INTTYPE, class CharType> Rational _strtorat(const CharType *s, CharType **end, INTTYPE (*strtointtype)(const CharType *, CharType **, int), int radix) {
  Rational result;
  EATWHITE();
  INTTYPE numerator = 0, denominator = 1;
  CharType *next = NULL;
  numerator = strtointtype(s, &next, radix);
  if(next == NULL) {
    return Rational::_0;
  }
  s = next;
  if(end) *end = next;
  if(*s == '/') {
    s++;
    next = NULL;
    denominator = strtointtype(s, &next, radix);
    if(next == NULL) {
      return (INT64)numerator;
    }
    s = next;
  }
  if(end) *end = (CharType*)s;
  return Rational((INT64)numerator, (INT64)denominator);
}

Rational strtorat(const char *s, char **end, int radix) {
  return (radix == 10) ? _strtorat<INT64, char>(s, end, strtoll, radix) : _strtorat<UINT64, char>(s, end, strtoull, radix);
}

Rational wcstorat(const wchar_t *s, wchar_t **end, int radix) {
  return (radix == 10) ? _strtorat<INT64, wchar_t>(s, end, wcstoll, radix) : _strtorat<UINT64, wchar_t>(s, end, wcstoull, radix);
}


char *rattoa(char *dst, const Rational &r, int radix) {
  if(!isfinite(r)) {
    return StrStream::formatUndefined(dst, _fpclass(r));
  }
  if(radix==10) {
    _i64toa(r.getNumerator(), dst, radix);
  } else {
    _ui64toa(r.getNumerator(), dst, radix);
  }
  if(!r.isInteger()) {
    strcat(dst, "/");
    char tmp[100];
    strcat(dst, _ui64toa(r.getDenominator(), tmp, radix));
  }
  return dst;
}

wchar_t *rattow(wchar_t *dst, const Rational &r, int radix) {
  if(!isfinite(r)) {
    return StrStream::formatUndefined(dst, _fpclass(r));
  }
  if(radix==10) {
    _i64tow(r.getNumerator(), dst, radix);
  } else {
    _ui64tow(r.getNumerator(), dst, radix);
  }
  if(!r.isInteger()) {
    wcscat(dst, L"/");
    wchar_t tmp[100];
    wcscat(dst, _ui64tow(r.getDenominator(), tmp, radix));
  }
  return dst;
}
