#pragma once
#include <Math/Double80.h>
#include <Math/Number.h>

namespace NumberStr {

inline void setNumberFloatValue(Number &n, const Double80 &v) {
  if(isFloat(v)) {
    n = (float)v;
  } else if(isDouble(v)) {
    n = (double)v;
  } else {
    n = v;
  }
}

#define setEnd(p) { if(end) *end=(p); }
template<typename CharType> Number parseNumber(const CharType *s, CharType **end, Double80(*tod80)(const CharType *, CharType **), Rational(*toRational)(const CharType *, CharType **, int), _locale_t locale) {
  CharType *afterD80 = NULL, *afterRat = NULL;
  const Double80 d80 = tod80(s, &afterD80);
  const Rational rat = toRational(s, &afterRat, 10);
  Number result;
  if(afterD80 == NULL) {
    if(afterRat == NULL) {
      return result;
    } else {
      result = rat;
      setEnd(afterRat);
    }
  } else if(afterRat == NULL) { // afterD80 != NULL
    setNumberFloatValue(result, d80);
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

}; // namespace NumberStr
