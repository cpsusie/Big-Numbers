#include "pch.h"
#include <limits.h>
#include <Math/Int128.h>
#include <Math/Rational.h>

// return random rational in range [0;1[
Rational randRational(UINT64 maxDenominator, RandomGenerator *rnd) {
  const INT64 den = randInt64(2, maxDenominator, rnd); // den in the range [2..maxDenominator]
  const INT64 num = randInt64(den, rnd);
  return Rational(num, den);
}

Rational randRational(const Rational &from, const Rational &to, UINT64 maxScaleFactor, RandomGenerator *rnd) {
  INT64 minNum = Rational::safeProd(__TFUNCTION__, __LINE__, from.getNumerator()  , to.getDenominator()   );
  INT64 maxNum = Rational::safeProd(__TFUNCTION__, __LINE__, to.getNumerator()    , from.getDenominator() );
  INT64 den    = Rational::safeProd(__TFUNCTION__, __LINE__, from.getDenominator(), to.getDenominator()   );
  if(maxScaleFactor > 1) {
    INT64 maxFactor = max(abs(minNum),abs(maxNum));
    maxFactor = max(maxFactor, den);
    INT64 scaleFactor = min(maxScaleFactor, (UINT64)(_I64_MAX / maxFactor));
    if(scaleFactor > 1) {
      scaleFactor = randInt64(2, scaleFactor, rnd);
      minNum *= scaleFactor; maxNum *= scaleFactor; den *= scaleFactor;
    }
  }
  const INT64 num = randInt64(minNum, maxNum, rnd);
  return Rational(num, den);
}
