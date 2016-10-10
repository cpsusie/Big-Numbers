#include "pch.h"

#define MAXDIGITS_INT64     19
#define MAXDIGITS_DIVISOR64 ((MAXDIGITS_INT64+1)/2)

class QuotL64Constants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;
  const ConstBigReal c4;

  QuotL64Constants()
    :c1(e(ConstBigReal(24),-2))
    ,c2(e(ConstBigReal( 4),-2))
    ,c3(e(ConstBigReal( 8),-2))
    ,c4(e(ConstBigReal(11),-1))
  {
  }
};

static const QuotL64Constants Q64C;

// Linear quot. Assume x != 0 and y != 0 and f != 0
BigReal BigReal::quotLinear64(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  const bool yNegative = y.isNegative();
  ((BigReal&)y).setPositive(); // cheating. We set it back agin

  const BigReal v = PAPCprod(<, PAPCprod(<, f, Q64C.c1, pool), y, pool); // v > 0

  BigReal z(pool);
  copy(z, x, v*Q64C.c2);

  z.setPositive();

  BigReal u(pool);
  u.approxQuot32(z, v);
  u.approxQuot32(v, getExpo10N(u)*Q64C.c3 + Q64C.c4);

  BRExpoType scale;
  const unsigned __int64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64,&scale);

  BigReal result(pool), t(pool), tmp(pool);
  bool loopDone = false;
  for(; compareAbs(z,v) > 0; loopDone = true) {
    t.approxQuot64Abs(z, yFirst, scale).m_negative = z.m_negative;
    result += t;
    z -= product(tmp, t, y, u, 0);
  }

  ((BigReal&)y).m_negative = yNegative;

  if(loopDone) {
    return result.setSignByProductRule(x,y);
  } else {
    return result.approxQuot64Abs(x, yFirst, scale).setSignByProductRule(x,y);
  }
}

// Assume x != 0 and y != 0
BigReal &BigReal::approxQuot64(const BigReal &x, const BigReal &y) {
  BRExpoType scale;
  const unsigned __int64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64, &scale);
  return approxQuot64Abs(x, yFirst, scale).setSignByProductRule(x,y);
}

// Assume x != 0 and y != 0
BigReal &BigReal::approxQuot64Abs(const BigReal &x, const unsigned __int64 &y, BRExpoType scale) {
  const unsigned __int64 q = x.getFirst64(MAXDIGITS_INT64)/y;
  *this = q;
  return multPow10(getExpo10(x) - scale - MAXDIGITS_INT64);
}

// Same as getFirst32, but k = [0..MAXDIGITS_INT64] = [0..19]
unsigned __int64 BigReal::getFirst64(const UINT k, BRExpoType *scale) const {
#ifdef _DEBUG
  DEFINEMETHODNAME;
  if(k > MAXDIGITS_INT64) {
    throwBigRealInvalidArgumentException(method, _T("k=%d. Legal interval is [0..%d]"),k,MAXDIGITS_INT64);
  }
#endif

  const Digit *p = m_first;
  if(p == NULL) {
    if(scale) *scale = 0;
    return 0;
  }

  int              tmpScale = 0;
  unsigned __int64 result   = p->n;
  int              digits   = getDecimalDigitCount(p->n), firstDigits = digits;
  if((UINT)digits >= k) {
    result /= pow10(digits-k); // digits-k <= LOG10_BIGREALBASE, so pow10 will not fail
    if(scale) {
      while(result % 10 == 0) {
        result /= 10;
        tmpScale++;
      }
    }
  } else { // digits < k
    if(scale) {
      for(p = p->next; (UINT)digits < k; digits += LOG10_BIGREALBASE) {
        if(p) {
          const BRDigitType p10 = pow10(min(LOG10_BIGREALBASE,k-digits));
          result = result * p10 + p->n / (BIGREALBASE/p10);
          p = p->next;
        } else {
          tmpScale = k - digits;
          break;
        }
      }
      while(result % 10 == 0) {
        result /= 10;
        tmpScale++;
      }
    } else { // scale == NULL
      for(p = p->next; (UINT)digits < k; digits += LOG10_BIGREALBASE) {
        const BRDigitType p10 = pow10(min(LOG10_BIGREALBASE,k-digits));
        result *= p10;
        if(p) {
          result += p->n / (BIGREALBASE/p10);
          p = p->next;
        }
      }
    }
  }

  if(scale) {
    *scale  = m_expo * LOG10_BIGREALBASE + firstDigits - 1 + tmpScale - k;
  }
  return result;
}

//#define TRACE_QUOTREMAINDER

void quotRemainder64(const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder) {
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
  const unsigned __int64 yFirst       = y.getFirst64(MAXDIGITS_DIVISOR64,&scale);
  const int              yDigits      = BigReal::getDecimalDigitCount64(yFirst);

  BigReal q(pool), t(pool), tmp(pool);
  while(compareAbs(z, y) >= 0) {
    t.approxQuot64Abs(z, yFirst, scale).m_negative = z.m_negative;
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

BigReal modulusOperator64(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal remainder(pool);
  quotRemainder64(x,y, NULL, &remainder);
  return remainder;
}

