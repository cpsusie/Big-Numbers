#include "pch.h"
#include <limits.h>

// Calculates with relative precision ie. with the specified number of decimal digits

#define _1 pool->get1()

BigReal rSum(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  const BRExpoType xe = BigReal::getExpo10(x);
  const BRExpoType ye = BigReal::getExpo10(y);
  return sum(x,y,e(_1,max(xe,ye) - (BRExpoType)digits - 2),pool);
}

BigReal rDif(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  const BRExpoType xe = BigReal::getExpo10(x);
  const BRExpoType ye = BigReal::getExpo10(y);
  return dif(x,y,e(_1,max(xe,ye) - (BRExpoType)digits - 2),pool);
}

BigReal rProd(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  return prod(x,y,e(_1,BigReal::getExpo10(x) + BigReal::getExpo10(y) - (BRExpoType)digits - 2),pool);
}

BigReal rQuot(const BigReal &x, const BigReal &y, size_t digits, DigitPool *digitPool) {
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  return quot(x,y,e(_1,BigReal::getExpo10(x) - BigReal::getExpo10(y) - (BRExpoType)digits - 2),pool);
}

BigReal rSqrt(const BigReal &x, size_t digits) {
  DigitPool *pool = x.getDigitPool();
  return sqrt(x,e(_1, BigReal::getExpo10(x)/2 - (BRExpoType)digits));
}

class rExpConstants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;

  rExpConstants()
    :c1(_T("0.43429448190325183")) // 0.434294481903251827 = 1/ln(10) round up
    ,c2(_T("0.43429448190325182")) //                        1/ln(10) round down
  {
  }
};

static const rExpConstants REXPC;

#ifdef IS32BIT
#define CD_MAX INT_MAX
#define CD_MIN INT_MIN
#else
#define CD_MAX LLONG_MAX
#define CD_MIN LLONG_MIN
#endif // IS32BIT

BigReal rExp(const BigReal &x, size_t digits) {
  DEFINEMETHODNAME;
  DigitPool *pool = x.getDigitPool();
  const BigReal c = x.isNegative() ? -rProd(REXPC.c1,fabs(x),20, pool) : rProd(REXPC.c2,x,20,pool);
  double cd;
  if(compareAbs(x, pool->getHalf()) < 0) { // prevent underflow in getDouble
    cd = 0;
  } else {
    cd = getDouble(c);
    if(cd > CD_MAX) {
      throwBigRealInvalidArgumentException(method, _T("Argument too big"));
    } else if(cd < CD_MIN) {
      throwBigRealInvalidArgumentException(method, _T("Argument too small"));
    }
  }
  return exp(x, e(_1, (intptr_t)cd - digits - 1));
}

static BigReal getLogError(const BigReal &x, intptr_t digits) {
  static const BigReal &c1 = BIGREAL_2;
  static const BigReal &c2 = BIGREAL_HALF;

  DigitPool *pool = x.getDigitPool();

  if(x > c1) {              // 2 < x
    return e(BigReal(BigReal::getExpo10(x)+1,pool),-digits,pool);
  } else if(x > BIGREAL_1) { // 1 < x <= 2
    return e(_1, BigReal::getExpo10((x-_1)) - digits,pool);
  } else if(x > c2) {       // 0.5 < x <= 1
    return e(_1, BigReal::getExpo10((_1-x)) - digits,pool);
  } else {                  // x <= 0.5
    return e(-BigReal::getExpo10N(x), -digits,pool);
  }
}

BigReal rLn(const BigReal &x, size_t digits) {
  return ln(x, getLogError(x, digits));
}

BigReal rLog(const BigReal &base, const BigReal &x, size_t digits) { // log(x) base
  BigReal lBase = rLn(base, digits+8);
  BigReal lX    = rLn(x   , digits+8);
  return rQuot(lX,lBase,digits+8);
}

BigReal rLog10(const BigReal &x, size_t digits) { // log(x) base 10
  return log10(x, getLogError(x, digits));
}

BigReal rPow(const BigReal &x, const BigReal &y, size_t digits) { // x^y
  DEFINEMETHODNAME;
  DigitPool *pool = x.getDigitPool();
  if(y.isZero()) {
    return _1;
  }
  if(y == BIGREAL_1) {
    return x;
  }
  if(x.isZero()) {
    if(y.isNegative()) {
      throwBigRealInvalidArgumentException(method, _T("x = 0 and y < 0"));
    }
    return pool->get0();
  }
  if(x.isPositive()) {
    return rExp(rProd(y,rLn(x,digits+8),digits+8,pool),digits+8);
  }

  if(even(y)) {
    return rPow(-x,y,digits);
  }
  if(odd(y)) {
    return -rPow(-x,y,digits);
  }

  throwBigRealInvalidArgumentException(method, _T("x < 0 and y not integer"));
  return BIGREAL_0;
}

BigReal rRoot(const BigReal &x, const BigReal &y, size_t digits) {
  DEFINEMETHODNAME;
  DigitPool *pool = x.getDigitPool();
  if(!x.isNegative()) {
    return rPow(x,rQuot(_1,y,digits),digits);
  }
  if(odd(y)) {
    return -rPow(-x,rQuot(_1,y,digits),digits);
  }

  throwBigRealInvalidArgumentException(method, _T("x < 0 and y not odd"));
  return BIGREAL_0;
}

BigReal rSin(const BigReal &x, size_t digits) {
  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->get0();
  }

  const BRExpoType piee = min(0,BigReal::getExpo10(x)) - (x.getDecimalDigits() + digits); // Exponent of precision of pi-value
  const BigReal Pi = pi(e(_1,piee - 8),pool);
  const BigReal xp = x % Pi;

  return sin(x,e(_1,BigReal::getExpo10(xp) - digits));
}

BigReal rCos(const BigReal &x, size_t digits) {
  static const BigReal &c1 = BIGREAL_HALF;

  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return _1;
  }

  const BRExpoType piee = -((intptr_t)x.getDecimalDigits() + (intptr_t)digits); // Exponent of precision of pi-value
  const BigReal Pih = pi(e(_1,piee - 8),pool) * c1;
  const BigReal xp = x % Pih;

  return cos(x,e(_1,BigReal::getExpo10(xp) - digits));
}

BigReal rTan(const BigReal &x, size_t digits) {
  const BigReal sn = rSin(x,digits);
  const BigReal cs = rCos(x,2*digits);
  return rQuot(sn,cs,digits);
}

BigReal rCot(const BigReal &x, size_t digits) {
  const BigReal cs = rCos(x,digits);
  const BigReal sn = rSin(x,2*digits);
  return rQuot(cs,sn,digits);
}

BigReal rAsin(const BigReal &x, size_t digits) {
  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->get0();
  }
  return asin(x,e(_1, min(0,BigReal::getExpo10(x)) - digits - 8));
}

BigReal rAcos(const BigReal &x, size_t digits) {
  DigitPool *pool = x.getDigitPool();
  if(x == BIGREAL_1) {
    return pool->get0();
  }
  return acos(x,e(_1, BigReal::getExpo10(_1 - x)/2 - digits - 8));
}

BigReal rAtan(const BigReal &x, size_t digits) {
  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->get0();
  }
  return atan(x,e(_1, min(0,BigReal::getExpo10(x)) - digits - 8));
}

BigReal rAcot(const BigReal &x, size_t digits) {
  static const BigReal &c1 = BIGREAL_HALF;
  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pi(e(_1,-(int)digits - 8), pool) * c1;
  }
  return acot(x,e(_1, min(0,-BigReal::getExpo10(x)) - digits - 8));
}

BigReal rRound(const BigReal &x, size_t digits) {
  if(x.isZero()) {
    return x;
  }
  return round(x, digits-BigReal::getExpo10(x)-1);
}
