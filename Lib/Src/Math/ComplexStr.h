#pragma once
#include <Math/Complex.h>

namespace ComplexStr {

#define EATWHITE() { while(iswspace(*s)) s++; }

template<typename CharType> Complex parseComplex(const CharType *s, CharType **end, Real(*toReal)(const CharType *, CharType **)) {
  bool ok = true;
  Real re = 0, im = 0;
  EATWHITE();
  if(*s == '(') {
    s++;
    EATWHITE();
    CharType *s1 = nullptr;
    re = toReal(s, &s1);
    s = s1;
    if(s) {
      EATWHITE();
      if(*s == ',') {
        s++;
        EATWHITE();
        s1 = nullptr;
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
    CharType *s1 = nullptr;
    re = toReal(s, &s1);
    if(s1 == nullptr) {
      ok = false;
    } else {
      s = s1;
    }
  }
  if(!ok) {
    return Complex::_0;
  }
  if(end) {
    *end = (CharType *)s;
  }
  return Complex(re, im);
}

template<typename CharType> CharType *complextostr(CharType *dst, const Complex &c, CharType* (*realToStr)(CharType *, Real x)) {
  if(c.im == 0) {
    return realToStr(dst, c.re);
  } else {
    CharType *tmp = dst;
    *(tmp++) = '(';
    CharType buf[50];
    strCpy(tmp, realToStr(buf, c.re));
    tmp += strLen(buf);
    *(tmp++) = ',';
    strCpy(tmp, realToStr(buf, c.im));
    tmp += strLen(buf);
    *(tmp++) = ')';
    *tmp = 0;
    return dst;
  }
}

}; // namespace ComplexStr
