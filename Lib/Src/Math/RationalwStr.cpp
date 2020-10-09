#include "pch.h"
#include "RationalStr.h"

using namespace RationalStr;

Rational wcstorat(const wchar_t *s, wchar_t **end, int radix) {
  return (radix == 10) ? parseRational<INT64, wchar_t>(s, end, wcstoll, radix) : parseRational<UINT64, wchar_t>(s, end, wcstoull, radix);
}

wchar_t *rattow(wchar_t *dst, const Rational &r, int radix) {
  if(!isfinite(r)) {
    return formatUndefined(dst, _fpclass(r));
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
