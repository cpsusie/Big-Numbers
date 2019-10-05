#include "pch.h"
#include <Math/Number.h>

static inline void setNumberFloatValue(Number &n, const Double80 &v) {
  if(isFloat(v)) {
    n = getFloat(v);
  } else if(isDouble(v)) {
    n = getDouble(v);
  } else {
    n = v;
  }
}

#define setEnd(p) { if(end) *end=(p); }
template<class CharType> Number _strtonum(const CharType *s, CharType **end, Double80 (*tod80)(const CharType *,CharType **), Rational (*toRational)(const CharType *,CharType **, int), _locale_t locale) {
  CharType *afterD80 = NULL, *afterRat = NULL;
  const Double80 d80 = tod80(     s, &afterD80);
  const Rational rat = toRational(s, &afterRat ,10);
  Number result;
  if(afterD80 == NULL) {
    if(afterRat == NULL) {
      return result;
    } else {
      result = rat;
      setEnd(afterRat);
    }
  } else if(afterRat == NULL) { // afterD80 != NULL
    setNumberFloatValue(result,d80);
    setEnd(afterD80);
  } else { // afterD80 != NULL && afterRat != NULL
    if(afterRat > afterD80) { // must be rational
      result = rat;
      setEnd(afterRat);
    } else {
      setNumberFloatValue(result, d80);
      setEnd(afterD80);
    }
  }
  return result;
}

Number _strtonum_l(const char    *s, char    **end, _locale_t locale) {
  return _strtonum(s, end, strtod80, strtorat, locale);
}

Number _wcstonum_l(const wchar_t *s, wchar_t **end, _locale_t locale) {
  return _strtonum(s, end, wcstod80, wcstorat, locale);
}

char *numtoa(char *dst, const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return flttoa(dst, getFloat(   n));
  case NUMBERTYPE_DOUBLE  : return dbltoa(dst, getDouble(  n));
  case NUMBERTYPE_DOUBLE80: return d80toa(dst, getDouble80(n));
  case NUMBERTYPE_RATIONAL: return rattoa(dst, getRational(n), 10);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return dst;
}

wchar_t *numtow(wchar_t *dst, const Number &n) {
  switch(n.getType()) {
  case NUMBERTYPE_FLOAT   : return flttow(dst, getFloat(   n));
  case NUMBERTYPE_DOUBLE  : return dbltow(dst, getDouble(  n));
  case NUMBERTYPE_DOUBLE80: return d80tow(dst, getDouble80(n));
  case NUMBERTYPE_RATIONAL: return rattow(dst, getRational(n), 10);
  default                 : n.throwUnknownTypeException(__TFUNCTION__);
  }
  return dst;
}
