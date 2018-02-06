#include "pch.h"
#include <limits.h>
#include <Math/Rational.h>

#define EATWHITE() { while(iswspace(*s)) s++; }

template<class CharType> Rational _strtorat(const CharType *s, CharType **end, INT64 (*strtoi)(const CharType *, CharType **, int), int radix) {
  DEFINEMETHODNAME;
  Rational result;
  EATWHITE();
  INT64 numerator = 0, denominator = 1;
  CharType *next = NULL;
  numerator = strtoi(s, &next, radix);
  if(next == NULL) {
    return 0;
  }
  s = next;
  if(end) *end = next;
  EATWHITE();
  if(*s == '/') {
    s++;
    EATWHITE();
    next = NULL;
    denominator = strtoi(s, &next, radix);
    if(next == NULL) {
      return numerator;
    }
    s = next;
  }
  if(end) *end = (CharType*)s;
  return Rational(numerator, denominator);
}

Rational strtorat(const char *s, char **end, int radix) {
  return _strtorat<char>(s, end, strtoll, radix);
}

Rational wcstorat(const wchar_t *s, wchar_t **end, int radix) {
  return _strtorat<wchar_t>(s, end, wcstoll, radix);
}

char *rattoa(char *dst, const Rational &r, int radix) {
  if(isNan(r)) {
    if(!isInfinity(r)) {
      return strcpy(dst, "-nan(ind)");
    } else if(isPInfinity(r)) {
      return strcpy(dst, "inf");
    } else if(isNInfinity(r)) {
      return strcpy(dst, "-inf");
    }
  }
  _i64toa(r.getNumerator(), dst, radix);
  if(r.getDenominator() != 1) {
    __assume(radix);
    strcat(dst, "/");
    char tmp[100];
    strcat(dst, _i64toa(r.getDenominator(), tmp, radix));
  }
  return dst;
}

wchar_t *rattow(wchar_t *dst, const Rational &r, int radix) {
  if(isNan(r)) {
    if(!isInfinity(r)) {
      return wcscpy(dst, L"-nan(ind)");
    } else if(isPInfinity(r)) {
      return wcscpy(dst, L"inf");
    } else if(isNInfinity(r)) {
      return wcscpy(dst, L"-inf");
    }
  }
  _i64tow(r.getNumerator(), dst, radix);
  if(r.getDenominator() != 1) {
    __assume(radix);
    wcscat(dst, L"/");
    wchar_t tmp[100];
    wcscat(dst, _i64tow(r.getDenominator(), tmp, radix));
  }
  return dst;
}
