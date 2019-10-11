#include "pch.h"
#include <limits.h>
#include <Math/Int128.h>
#include <Math/Rational.h>

// return random rational in range [0;1[
Rational randRational(UINT64 maxDenominator, RandomGenerator *rnd) {
  const INT64 den = randInt64(maxDenominator-1, rnd)+2; // den in the range [2..maxDenominator]
  const INT64 num = randInt64(den, rnd);
  return Rational(num, den);
}

Rational randRational(const Rational &low, const Rational &high, RandomGenerator *rnd) {
  const INT64 l   = Rational::safeProd(__TFUNCTION__,__LINE__,low.getNumerator() , high.getDenominator());
  const INT64 r   = Rational::safeProd(__TFUNCTION__,__LINE__,high.getNumerator(), low.getDenominator());
  const INT64 num = randInt64(l, r, rnd);
  const INT64 den = Rational::safeProd(__TFUNCTION__,__LINE__,low.getDenominator(),high.getDenominator());
  return Rational(num, den);
}
