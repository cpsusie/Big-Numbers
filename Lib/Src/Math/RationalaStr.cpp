#include "pch.h"
#include <limits.h>
#include "RationalStr.h"

using namespace RationalStr;

Rational strtorat(const char *s, char **end, int radix) {
  return (radix == 10) ? parseRational<INT64, char>(s, end, strtoll, radix) : parseRational<UINT64, char>(s, end, strtoull, radix);
}

char *rattoa(char *dst, const Rational &r, int radix) {
  if(!isfinite(r)) {
    return formatUndefined(dst, _fpclass(r));
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
