#pragma once

#include <Math/Rational.h>
#include <StrStream.h>

namespace RationalStr {

using namespace OStreamHelper;

#define EATWHITE() { while(iswspace(*s)) s++; }

template<typename INTTYPE, typename CharType> Rational parseRational(const CharType *s, CharType **end, INTTYPE(*strtointtype)(const CharType *, CharType **, int), int radix) {
  Rational result;
  EATWHITE();
  INTTYPE numerator = 0, denominator = 1;
  CharType *next = NULL;
  errno = 0;
  numerator = strtointtype(s, &next, radix);
  if(next == NULL) {
    return Rational::_0;
  }
  s = next;
  if(end) *end = next;
  if(*s == '/') {
    s++;
    next = NULL;
    denominator = strtointtype(s, &next, radix);
    if(next == NULL) {
      return (INT64)numerator;
    }
    s = next;
  }
  if(end) *end = (CharType *)s;
  return Rational((INT64)numerator, (INT64)denominator);
}

// assume !r.isInteger()
// Return dst
String &formatRational(String &dst, const Rational &r, StreamParameters &param);

}; // namespace RationalStr
