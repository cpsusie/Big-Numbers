#include "pch.h"

static const ConstBigReal modulusC1 = e(BigReal::_1,-1);

BigReal oldFraction(const BigReal &x) { // sign(x) * (|x| - floor(|x|))
  if(x.isZero()) {
    return x;
  }
  if(isInteger(x)) {
    return x.getDigitPool()->_0();
  }
  if(x.isNegative()) {
    const BigReal xp(fabs(x));
    return floor(xp) - xp;
  } else {
    return x - floor(x);
  }
}

void quotRemainder(const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder) {
  BigReal::validateQuotRemainderArguments(__TFUNCTION__, x, y, quotient, remainder);
  if(!BigReal::checkIsNormalQuotient(x, y, quotient, remainder)) return;

  DigitPool *pool   = x.getDigitPool();
  const int  cmpAbs = BigReal::compareAbs(x, y);
  if(cmpAbs < 0) {
    if(remainder) *remainder = x;
    if(quotient )  quotient->setToZero();
    return;
  } else if(cmpAbs == 0) {
    if(remainder) remainder->setToZero();
    if(quotient ) {
      *quotient = quotient->getDigitPool()->_1();
    }
    return;
  }

  BigReal tmpX(x);
  tmpX.setPositive();
  BigReal tmpY(y);
  tmpY.setPositive();
  BigInt  q   = floor(quot(tmpX, tmpY, modulusC1));
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

  if(remainder) {
    remainder->copySign(x); // sign(x % y) = sign(x), equivalent to built-in % operator
  }
}
