#include "pch.h"
#include "NumberStr.h"

using namespace NumberStr;

Number _strtonum_l(const char    *s, char    **end, _locale_t locale) {
  return parseNumber(s, end, strtod80, strtorat, locale);
}

char *numtoa(char *dst, const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return flttoa(dst, (float   )n);
  case NUMBERTYPE_DOUBLE  : return dbltoa(dst, (double  )n);
  case NUMBERTYPE_DOUBLE80: return d80toa(dst, (Double80)n);
  case NUMBERTYPE_RATIONAL: return rattoa(dst, (Rational)n, 10);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return dst;
}

