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

Rational randRational(const Rational &low, const Rational &high, UINT64 maxScaleFactor, RandomGenerator *rnd) {
  INT64 numLow  = Rational::safeProd(__TFUNCTION__, __LINE__, low.getNumerator()  , high.getDenominator());
  INT64 numHigh = Rational::safeProd(__TFUNCTION__, __LINE__, high.getNumerator() , low.getDenominator() );
  INT64 den     = Rational::safeProd(__TFUNCTION__, __LINE__, low.getDenominator(), high.getDenominator());
  if(maxScaleFactor > 1) {
    INT64 maxFactor = max(abs(numLow),abs(numHigh));
    maxFactor = max(maxFactor, den);
    INT64 scaleFactor = min(maxScaleFactor, (UINT64)(_I64_MAX / maxFactor));
    if(scaleFactor > 1) {
      scaleFactor = randInt64(2, scaleFactor, rnd);
      numLow *= scaleFactor; numHigh *= scaleFactor; den *= scaleFactor;
    }
  }
  const INT64 num = randInt64(numLow, numHigh, rnd);
  return Rational(num, den);
}
