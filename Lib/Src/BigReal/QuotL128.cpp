#include "pch.h"

#ifdef IS64BIT

#define MAXDIGITS_INT128     38
#define MAXDIGITS_DIVISOR128 ((MAXDIGITS_INT128+1)/2)

class QuotL128Constants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;
  const ConstBigReal c4;

  QuotL128Constants()
    :c1(e(ConstBigReal(24),-2))
    ,c2(e(ConstBigReal( 4),-2))
    ,c3(e(ConstBigReal( 8),-2))
    ,c4(e(ConstBigReal(11),-1))
  {
  }
};

static const QuotL128Constants Q128C;

// Linear quot. Assume x != 0 and y != 0 and f != 0
BigReal BigReal::quotLinear128(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  const bool yNegative = y.isNegative();
  ((BigReal&)y).setPositive(); // cheating. We set it back agin

  const BigReal v = APCprod(<, APCprod(<, f, Q128C.c1, pool), y, pool); // v > 0

  BigReal z(pool);
  copy(z, x, v*Q128C.c2);

  z.setPositive();

  BigReal u(pool);
  u.approxQuot32(z, v);
  u.approxQuot32(v, getExpo10N(u)*Q128C.c3 + Q128C.c4);

  BRExpoType scale;
  _uint128 yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);

  BigReal result(pool), t(pool), tmp(pool);
  bool loopDone = false;
  for(; compareAbs(z,v) > 0; loopDone = true) {
    t.approxQuot128Abs(z, yFirst, scale).m_negative = z.m_negative;
    result += t;
    z -= product(tmp, t, y, u, 0);
  }

  ((BigReal&)y).m_negative = yNegative;

  if(loopDone) {
    return result.setSignByProductRule(x,y);
  } else {
    return result.approxQuot128Abs(x, yFirst, scale).setSignByProductRule(x,y);
  }
}

// Assume x != 0 and y != 0
BigReal &BigReal::approxQuot128(const BigReal &x, const BigReal &y) {
  BRExpoType scale;
  _uint128 yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);
  return approxQuot128Abs(x, yFirst, scale).setSignByProductRule(x,y);
}

// Assume x != 0 and y != 0
BigReal &BigReal::approxQuot128Abs(const BigReal &x, const _uint128 &y, BRExpoType scale) {
  _uint128 xFirst;
  const _uint128 q = x.getFirst128(xFirst, MAXDIGITS_INT128)/y;
  *this = q;
  return multPow10(getExpo10(x) - scale - MAXDIGITS_INT128);
}

// Same as getFirst32, but k = [0..MAXDIGITS_INT128] = [0..38]
_uint128 &BigReal::getFirst128(_uint128 &dst, const UINT k, BRExpoType *scale) const {
#ifdef _DEBUG
  DEFINEMETHODNAME;
  if(k > MAXDIGITS_INT128) {
    throwBigRealInvalidArgumentException(method, _T("k=%d. Legal interval is [0..%d]"),k,MAXDIGITS_INT128);
  }
#endif

  const Digit *p = m_first;
  if(p == NULL) {
    if(scale) *scale = 0;
    dst = 0;
    return dst;
  }

  int              tmpScale = 0;
  dst                       = p->n;
  int              digits   = getDecimalDigitCount(p->n), firstDigits = digits;
  if((UINT)digits >= k) {
    dst /= pow10(digits-k); // digits-k <= LOG10_BIGREALBASE, so pow10 will not fail
    if(scale) {
      while(dst % 10 == 0) {
        dst /= 10;
        tmpScale++;
      }
    }
  } else { // digits < k
    if(scale) {
      for(p = p->next; (UINT)digits < k; digits += LOG10_BIGREALBASE) {
        if(p) {
          const BRDigitType p10 = pow10(min(LOG10_BIGREALBASE,k-digits));
          dst = dst * p10 + p->n / (BIGREALBASE/p10);
          p = p->next;
        } else {
          tmpScale = k - digits;
          break;
        }
      }
      while(dst % 10 == 0) {
        dst /= 10;
        tmpScale++;
      }
    } else { // scale == NULL
      for(p = p->next; (UINT)digits < k; digits += LOG10_BIGREALBASE) {
        const BRDigitType p10 = pow10(min(LOG10_BIGREALBASE,k-digits));
        dst *= p10;
        if(p) {
          dst += p->n / (BIGREALBASE/p10);
          p = p->next;
        }
      }
    }
  }

  if(scale) {
    *scale  = m_expo * LOG10_BIGREALBASE + firstDigits - 1 + tmpScale - k;
  }
  return dst;
}

//#define TRACE_QUOTREMAINDER

void quotRemainder128(const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder) {
  DEFINEMETHODNAME;
  if(y.isZero()) {
    throwBigRealInvalidArgumentException(method, _T("Division by zero"));
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

  // x != 0 && y != 0 && |x| > |y|

  if(BigReal::isPow10(y)) {
    BigReal tmp(x);
    const BRExpoType yp10 = BigReal::getExpo10(y);
    tmp.multPow10(-yp10);
    tmp.fractionate(quotient, remainder);
    if(remainder) {
      remainder->multPow10(yp10);
    }
    if(quotient) {
      quotient->setPositive();
    }
    return;
  }

  const bool yNegative = y.isNegative();
  ((BigReal&)y).setPositive(); // cheating. We set it back agin

  DigitPool *pool = x.getDigitPool();

  BigReal z(x, pool);
  z.setPositive();

  BRExpoType             scale;
  _uint128               yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);
  const int              yDigits      = BigReal::getDecimalDigitCount64(yFirst);

  BigReal q(pool), t(pool), tmp(pool);
  while(compareAbs(z, y) >= 0) {
    t.approxQuot128Abs(z, yFirst, scale).m_negative = z.m_negative;
    q += t;
    z -= BigReal::product(tmp, t, y, pool->get0(),0);
  }

  if(!z.isNegative()) {
    if(isInteger(q)) {
      if(quotient)  *quotient  = q;
      if(remainder) *remainder = z;
    } else {
      q.fractionate(quotient, &t);
      z += t * y;
      if(compareAbs(z, y) >= 0) {
        if(quotient)  ++(*quotient);
        if(remainder) *remainder = z - y;
      } else {
        if(remainder) *remainder = z;
      }
    }
  } else { // z < 0
    if(isInteger(q)) {
      if(quotient)  {
        *quotient = q;
        --(*quotient);
      }
      if(remainder) {
        *remainder = z + y;
      }
    } else {
      if(!remainder) { // quotient != NULL
        q.fractionate(quotient, &t);
        z += t * y;
        if(z.isNegative()) {
          --(*quotient);
        }
      } else { // remainder != NULL. quotient might be NULL
        q.fractionate(quotient, &t);
        *remainder = z + t * y;
        if(remainder->isNegative()) {
          *remainder += y;
          if(quotient) {
            --(*quotient);
          }
        }
      }
    }
  }

  if(remainder) {
    remainder->m_negative = x.m_negative;
  }
  ((BigReal&)y).m_negative = yNegative;
}

BigReal modulusOperator128(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal remainder(pool);
  quotRemainder128(x,y, NULL, &remainder);
  return remainder;
}

#endif
