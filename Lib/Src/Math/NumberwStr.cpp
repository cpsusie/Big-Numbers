#include "pch.h"
#include "NumberStr.h"

using namespace NumberStr;

Number _wcstonum_l(const wchar_t *s, wchar_t **end, _locale_t locale) {
  return parseNumber(s, end, wcstod80, wcstorat, locale);
}

wchar_t *numtow(wchar_t *dst, const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return flttow(dst, (float   )n);
  case NUMBERTYPE_DOUBLE  : return dbltow(dst, (double  )n);
  case NUMBERTYPE_DOUBLE80: return d80tow(dst, (Double80)n);
  case NUMBERTYPE_RATIONAL: return rattow(dst, (Rational)n, 10);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return dst;
}
