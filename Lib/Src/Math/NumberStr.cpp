#include "pch.h"
#include <Math/Number.h>

template<class CharType> Number _strton(const CharType *s, CharType **end, Real (*toReal)(const CharType *,CharType **), Rational (*toRational)(const CharType *,CharType **, int)) {
  CharType *afterReal = NULL, *afterRat = NULL;
  const Real     r   = toReal(    s, &afterReal);
  const Rational rat = toRational(s, &afterRat ,10);
  Number result;
  if(afterReal == NULL) {
    if(afterRat == NULL) {
      return result;
    } else {
      result = rat;
      if(end) *end    = afterRat;
    }
  } else if(afterRat == NULL) { // afterReal != NULL
    result = r;
    if(end) *end    = afterReal;
  } else { // afterReal != NULL && afterRat != NULL
    if(afterRat > afterReal) { // must be rational
      result = rat;
      if(end) *end    = afterRat;
    } else {
      result = r;
      if(end) *end    = afterReal;
    }
  }
  return result;
}

Number strton(const char *s, char **end) {
  return _strton<char>(s, end, strtor, strtorat);
}

Number wcston(const wchar_t *s, wchar_t **end) {
  return _strton<wchar_t>(s, end, wcstor, wcstorat);
}

