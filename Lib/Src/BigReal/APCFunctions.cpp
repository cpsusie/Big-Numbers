#include "pch.h"

#define _1 pool->get1()

BigReal BigReal::apcSum(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();

  if(x.isZero()) {
    return BigReal(y, pool);
  } else if(y.isZero()) {
    return digitPool ? BigReal(x, digitPool) : x;
  }

  const BRExpoType xe = getExpo10(x);
  const BRExpoType ye = getExpo10(y);
  return sum(x, y, e(_1, max(xe, ye) - APC_DIGITS - 2), pool).rRound(APC_DIGITS+2).adjustAPCResult(bias, method);
}

BigReal BigReal::apcProd(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  if(x.isZero() || y.isZero()) {
    return pool->get0();
  }

  BigReal result(pool);
  return result.shortProduct(x, y, x.m_expo + y.m_expo).rTrunc(APC_DIGITS+2).adjustAPCResult(bias, method);
}

#define MAXDIGITS_INT64     19
#define MAXDIGITS_DIVISOR64 ((MAXDIGITS_INT64+1)/2)

BigReal BigReal::apcQuot(const char bias, const BigReal &x, const BigReal &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();

  if(x.isZero()) {
    return pool->get0();
  }

  const bool yNegative = y.isNegative();
  ((BigReal&)y).setPositive(); // cheating. We set it back agin

  BigReal z(pool);
  z.copyrTrunc(x, MAXDIGITS_INT64).setPositive();

  BRExpoType scale;
  const __int64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64, &scale);

  BigReal result(pool), tmp(pool), t(pool);
  for(int i = 0;; i++) {
    t.approxQuot64Abs(z, yFirst, scale).m_negative = z.m_negative;
    result += t;
    if(i == 1) break;
    z -= tmp.shortProductNoZeroCheck(t, y, 4);
    if(z.isZero()) break;
  }

  ((BigReal&)y).m_negative = yNegative;
  return result.rTrunc(APC_DIGITS+2).setSignByProductRule(x,y).adjustAPCResult(bias, method);
}

BigReal BigReal::apcPow(const char bias, const BigReal &x, const BigInt &y, DigitPool *digitPool) {
  DEFINEMETHODNAME;
  DigitPool           *pool = digitPool ? digitPool : x.getDigitPool();
  static const BigReal &c1  = BIGREAL_HALF;

  if(y.isZero()) {
    return _1;
  } else if(x.isZero()) {
    if(y.isNegative()) {
      throwBigRealInvalidArgumentException(method, _T("x = 0 and y < 0"));
    }
    return pool->get0();
  } else {
    BigReal result = _1;
    BigReal tmpX(x,pool), tmp(pool);

#define SHORTPROD(a,b) tmp.shortProductNoZeroCheck(a,b,4)

    const bool yNegative = y.isNegative();
    if(compareAbs(y, ConstBigReal::_ulong_max) <= 0) {       // use unsigned long as exponent;
      for(unsigned long tmpY = yNegative ? getUlong(-y) : getUlong(y); tmpY;) {
        if(tmpY & 1) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
        }
      }
    } else if(compareAbs(y, ConstBigReal::_ui64_max) <= 0) { // use unsigned __int64 as exponent
      for(unsigned __int64 tmpY = yNegative ? getUint64(-y) : getUint64(y); tmpY;) {
        if(tmpY & 1) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY /= 2;
        }
      }
    } else {                                                // use BigInt all the way down. guess this will almost never happen
      BigReal tmpY(y, pool);
      if(yNegative) tmpY.changeSign();
      while(!tmpY.isZero()) {
        if(odd(tmpY)) {
          result = SHORTPROD(result, tmpX); --tmpY;
        } else {
          tmpX   = SHORTPROD(tmpX  , tmpX); tmpY *= c1;
        }
      }
    }
    if(yNegative) {
      return apcQuot(bias, _1, result, pool);
    } else {
      return result.rTrunc(APC_DIGITS+2).adjustAPCResult(bias, method);
    }
  }
}

static void throwInvalidBias(const TCHAR *function, char bias) {
  throwBigRealException(_T("%s:Invalid bias:'%c'. Must be <,> or #"), function, bias);
}

BigReal &BigReal::adjustAPCResult(const char bias, const TCHAR *function) {
  if(bias == '#') {
    return *this;
  } else {
    BigReal f(m_digitPool.get1());
    f.multPow10(getExpo10(*this) - APC_DIGITS - 1);
    switch(bias) {
    case '<': *this -= f; break;
    case '>': *this += f; break;
    default : throwInvalidBias(function, bias);
    }
  }
  return *this;
}
