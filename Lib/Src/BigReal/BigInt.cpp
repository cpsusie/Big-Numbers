#include "pch.h"
#include <Math/BigInt.h>

BigInt::BigInt(const BigReal &x, DigitPool *digitPool) : BigReal(digitPool ? digitPool : x.getDigitPool()) {
  digitPool = getDigitPool();

  if(!isnormal(x)) {
    copyFields(x);
  } else if(x.m_expo < 0) { // |x| < 1
    *this = x.isNegative() ? -digitPool->get1() : digitPool->get0();
  } else if(x.getLow() >= 0) { // x is an integer
    copyDigits(x, x.getLength());
    copyFields(x);
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

BigInt::BigInt(const String &s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const char *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt::BigInt(const wchar_t *s, DigitPool *digitPool) : BigReal(digitPool) {
  init(s, false);
}

BigInt &BigInt::operator=(const BigReal &x) {
  BigReal::operator=((BigInt)x);
  return *this;
}

BigInt operator+(const BigInt &x, const BigInt &y) {
  return (BigInt)sum(x,y,x.getDigitPool()->get0());
}

BigInt operator-(const BigInt &x, const BigInt &y) {
  return (BigInt)dif(x,y,x.getDigitPool()->get0());
}

BigInt operator-(const BigInt &x) {
  if(x.isZero()) {
    return x;
  }
  BigInt result(x);
  result.m_negative = !result.m_negative;
  return result;
}

BigInt operator*(const BigInt &x, const BigInt &y) {
  return (BigInt)prod(x,y,x.getDigitPool()->get0());
}

BigInt operator%(const BigInt &x, const BigInt &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal result(pool);
  quotRemainder(x,  y, NULL, &result);
  return (BigInt)result;
}

static const ConstBigReal divisionC1 = e(BIGREAL_1,-1);

BigInt operator/(const BigInt &x, const BigInt &y) { // BigInt division
  BigInt tmpX(x);
  tmpX.setPositive();
  BigInt tmpY(y);
  tmpY.setPositive();
  BigInt result(round(quot(tmpX,tmpY,divisionC1)));
  BigInt chk(tmpY * result);
  if(chk > tmpX) {
    --result;
  }
  return (BigInt)result.setSignByProductRule(x,y);
}

BigInt &BigInt::operator/=(const BigInt &x) {
  return *this = *this / x;
}

BigInt randBigInt(size_t length, RandomGenerator *rnd, DigitPool *pool) {
  return BigInt(e(randBigReal(length, rnd, pool),length,pool),pool);
}
