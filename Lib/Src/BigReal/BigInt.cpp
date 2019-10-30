#include "pch.h"
#include <Math/BigInt.h>

BigInt::BigInt(const BigReal &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  digitPool = getDigitPool();

  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.m_negative);
  } else if(x.m_expo < 0) { // |x| < 1
    *this = x.isNegative() ? -digitPool->_1() : digitPool->_0();
  } else if(x.getLow() >= 0) { // x is an integer
    copyAllDigits(x);
    copyNonPointerFields(x);
  } else { // |x| > 1 and x is not an integer
    copyDigits(x, (m_expo = x.m_expo)+1);
    m_low      = 0;
    m_negative = x.m_negative;
    trimZeroes();
    if(m_negative) {
      --(*this);
    }
  }
}

BigInt::BigInt(const BigInt &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  if(!x._isnormal()) {
    setToNonNormal(x.m_low, x.m_negative);
  } else {
    copyAllDigits(x);
    copyNonPointerFields(x);
  }
}

BigInt::BigInt(const String &s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const char *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const wchar_t *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt quot(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = x.getDigitPool();
  BigInt result(digitPool);
  quotRemainder(x, y, &result, NULL);
  result.setSignByProductRule(x, y);
  return result;
}

BigInt rem(const BigInt &x, const BigInt &y, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = x.getDigitPool();
  BigInt result(digitPool);
  quotRemainder(x, y, NULL, &result);
  return result;
}

// Return uniform distributed random BigInt in [0..e(1,maxDigits)-1], digits generated with rnd.
// If digitPool == NULL, use DEFAULT_DIGITPOOL
// ex:maxDigits = 3:returned values in interval [0..999]
BigInt randBigInt(size_t maxDigits, RandomGenerator *rnd, DigitPool *digitPool) {
  return BigInt(e(randBigReal(maxDigits, rnd, digitPool),maxDigits));
}

// Return uniform distributed random BigInt in [0..n-1], digits generated with rnd.
// If digitPool == NULL, use n.getDigitPool()
BigInt randBigInt(const BigInt &n, RandomGenerator *rnd, DigitPool *digitPool) {
  if(digitPool == NULL) digitPool = n.getDigitPool();
  if(!n._isnormal()) return BigInt(digitPool->nan());
  return randBigInt(BigReal::getExpo10(n), rnd, digitPool) % n;
}

// Return uniform distributed random BigInt in [from;to], digits generated with rnd.
// If digitPool == NULL, use from.getDigitPool()
inline BigInt randBigInt(const BigInt &from, const BigInt &to, RandomGenerator *rnd, DigitPool *digitPool) {
  if(from >= to) {
    throwBigRealInvalidArgumentException(__TFUNCTION__, _T("from >= to"));
  }
  if(digitPool == NULL) digitPool = from.getDigitPool();
  if(!from._isfinite() || !to._isfinite()) return BigInt(digitPool->nan());
  const BigInt result = randBigInt(sum(dif(to, from, digitPool), BigReal::_1), rnd); // r in [0..to-from]
  return result + from;
}
