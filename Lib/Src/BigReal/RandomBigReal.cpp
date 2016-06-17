#include "pch.h"

#ifdef IS32BIT
#define nextDigit nextInt
#else
#define nextDigit nextInt64
#endif

// 0 <= random < 1; with the specified number of decimal digits
BigReal BigReal::random(size_t length, Random *rnd, DigitPool *digitPool) { // static
  Random    &r    = rnd       ? *rnd      : _standardRandomGenerator;
  DigitPool *pool = digitPool ? digitPool : &DEFAULT_DIGITPOOL;
  if(length == 0) {
    return pool->get0();
  }
  BigReal        result(pool);
  const intptr_t wholeDigits = length/LOG10_BIGREALBASE;
  intptr_t       i;

  for(i = 0; i < wholeDigits; i++) {
    result.appendDigit(r.nextDigit(BIGREALBASE));
  }

  if(length % LOG10_BIGREALBASE) {
    const BRDigitType s = BigReal::pow10(length % LOG10_BIGREALBASE);
    result.appendDigit(r.nextDigit(s) * (BIGREALBASE / s));
    i++;
  }

  result.m_expo     = -1;
  result.m_low      = -i;
  result.setPositive();
  result.trimZeroes();
  SETBIGREALDEBUGSTRING(result);
  return result;
}

  // Return uniform distributed random number between 0 (incl) and 1 (excl)
  // with length decimal digits. If digitPool == NULL, use DEFAULT_DIGITPOOL
BigReal  RandomBigReal::nextBigReal(size_t length, DigitPool *digitPool) { 
  return BigReal::random(length, this, digitPool);
}

  // Return uniform distributed random number between low (incl) and high (excl)
  // with length decimal digits. If digitPool == NULL, use DEFAULT_DIGITPOOL
BigReal  RandomBigReal::nextBigReal(const BigReal &low, const BigReal &high, size_t length, DigitPool *digitPool) {
  DEFINEMETHODNAME(nextBigReal);
  if(low >= high) {
    throwBigRealInvalidArgumentException(method, _T("low >= high"));
  }
  DigitPool *pool = digitPool ? digitPool : low.getDigitPool();
  BigReal     r    = nextBigReal(length, pool);
  BigReal f(e(pool->get1(), BigReal::getExpo10(low) - length));
  r = sum(prod(high-low, r, f, pool), low, f, pool);
  return (r.isZero() || (r.getDecimalDigits() <= length) || (length == 0)) ? r : r.rTrunc(length);
}

  // Return uniform distributed random BigInt in range [0..10^length[
BigInt RandomBigReal::nextInteger(size_t length, DigitPool *digitPool) {
  return randomInteger(length, this, digitPool);
}
