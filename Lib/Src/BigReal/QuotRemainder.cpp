#include "pch.h"

static const ConstBigReal modulusC1 = e(BIGREAL_1,-1);

BigReal oldFraction(const BigReal &x) { // sign(x) * (|x| - floor(|x|))
  if(x.isZero()) {
    return x;
  }
  if(isInteger(x)) {
    return x.getDigitPool()->get0();
  }
  if(x.isNegative()) {
    const BigReal xp(fabs(x));
    return floor(xp) - xp;
  } else {
    return x - floor(x);
  }
}

void quotRemainder(const BigReal &x,  const BigReal &y, BigInt *quotient, BigReal *remainder) {
  DEFINEMETHODNAME;
  if(y.isZero()) {
    throwBigRealInvalidArgumentException(method, _T("Division by zero. (%s / 0)."), x.toString().cstr());
  }
  if(quotient == remainder) { // also takes care of the stupid situation where both are NULL
    throwBigRealInvalidArgumentException(method, _T("quotient is the same variable as remainder"));
  }
  if(quotient == &x || quotient == &y) {
    throwBigRealInvalidArgumentException(method, _T("quotient cannot be the same variable as x or y"));
  }
  if(remainder == &x || remainder == &y) {
    throwBigRealInvalidArgumentException(method, _T("remainder cannot be the same variable as x or y"));
  }

  if(x.isZero()) {
    if(quotient ) quotient->setToZero();
    if(remainder) remainder->setToZero();
    return;
  }

  DigitPool *pool = x.getDigitPool();
  const int cmpAbs = compareAbs(x, y);
  if(cmpAbs < 0) {
    if(remainder) *remainder = x;
    if(quotient)  quotient->setToZero();
    return;
  } else if(cmpAbs == 0) {
    if(remainder) remainder->setToZero();
    if(quotient) {
      *quotient = quotient->getDigitPool()->get1();
    }
    return;
  }

  BigReal tmpX(x);
  tmpX.setPositive();
  BigReal tmpY(y);
  tmpY.setPositive();
  BigInt q   = floor(quot(tmpX, tmpY, modulusC1));
  BigReal mod = tmpX - q * tmpY;

  if(mod.isNegative()) {
    if(remainder) {
      *remainder = mod + tmpY;
    }
    if(quotient) {
      --q;
      *quotient = q;
    }
  } else if(mod >= tmpY) {
    if(remainder) {
      *remainder = mod - tmpY;
    }
    if(quotient) {
      ++q;
      *quotient = q;
    }
  } else {
    if(remainder) {
      *remainder = mod;
    }
    if(quotient) {
      *quotient = q;
    }
  }

  if(remainder && (remainder->m_negative != x.m_negative)) {
    remainder->m_negative = x.m_negative; // sign(x % y) = sign(x), equivalent to build-in % operator
  }
}
