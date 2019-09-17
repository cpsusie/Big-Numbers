#include "pch.h"
#include <Math/Complex.h>

#define EATWHITE() { while(iswspace(*s)) s++; }

template<class CharType> Complex _strtoc(const CharType *s, CharType **end, Real (*toReal)(const CharType *,CharType **)) {
  bool ok = true;
  Real re = 0, im = 0;
  EATWHITE();
  if(*s == '(') {
    s++;
    EATWHITE();
    CharType *s1 = NULL;
    re = toReal(s, &s1);
    s = s1;
    if(s) {
      EATWHITE();
      if(*s == ',')  {
        s++;
        EATWHITE();
        s1 = NULL;
        im = toReal(s, &s1);
        s = s1;
        if(s) {
          EATWHITE();
        }
      }
    }
    if(s) {
      if(*s != ')') {
        ok = false;
      } else {
        s++;
      }
    }
  } else {
    CharType *s1 = NULL;
    re = toReal(s, &s1);
    if(s1 == NULL) {
      ok = false;
    } else {
      s = s1;
    }
  }
  if(!ok) {
    return Complex::zero;
  }
  if(end) {
    *end = (CharType*)s;
  }
  return Complex(re,im);
}

Complex strtoc(const char *s, char **end) {
  return _strtoc(s, end, strtor);
}

Complex wcstoc(const wchar_t *s, wchar_t **end) {
  return _strtoc(s, end, wcstor);
}
