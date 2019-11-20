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
    :c1(e(BigReal(24),-2))
    ,c2(e(BigReal( 4),-2))
    ,c3(e(BigReal( 8),-2))
    ,c4(e(BigReal(11),-1))
  {
  }
};

static const QuotL64Constants Q64C;

// Assume isNormalQuotient(x, y) and f>0.
// x/y with |error| < f. School method. using built-in 64-bit division
BigReal BigReal::quotLinear64(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  assert(isNormalQuotient(x, y) && f.isPositive());

  const BigReal v = APCprod(<, APCprod(<, f, Q64C.c1, pool), APCabs(#,y,pool), pool); // v > 0

  BigReal z(pool);
  copy(z, x, v*Q64C.c2).clrFlags(BR_NEG);

  BigReal u(pool);
  u.approxQuot32(z, v);
  u.approxQuot32(v, getExpo10N(u)*Q64C.c3 + Q64C.c4);

  BRExpoType scale;
  const UINT64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64,&scale);

  BigReal result(pool), t(pool), tmp(pool);
  bool loopDone = false;
  for(; compareAbs(z,v) > 0; loopDone = true) {
    t.approxQuot64Abs(z, yFirst, scale).copySign(z);
    result += t;
    z -= product(tmp, t, y, u, 0).copySign(t);
  }

  if(loopDone) {
    return result.setSignByProductRule(x,y);
  } else {
    return result.approxQuot64Abs(x, yFirst, scale).setSignByProductRule(x,y);
  }
}

BigReal &BigReal::approxQuot64(const BigReal &x, const BigReal &y) {
  assert(x._isfinite() && y._isnormal());
  BRExpoType scale;
  const unsigned __int64 yFirst = y.getFirst64(MAXDIGITS_DIVISOR64, &scale);
  return approxQuot64Abs(x, yFirst, scale).setSignByProductRule(x,y);
}

BigReal &BigReal::approxQuot64Abs(const BigReal &x, const unsigned __int64 &y, BRExpoType scale) {
  assert(x._isfinite() && (y != 0));
  const unsigned __int64 q = x.getFirst64(MAXDIGITS_INT64)/y;
  *this = q;
  return multPow10(getExpo10(x) - scale - MAXDIGITS_INT64,true);
}

UINT64 BigReal::getFirst64(const UINT k, BRExpoType *scale) const {
  assert(k <= MAXDIGITS_INT64);
  const Digit *p = m_first;
  if(p == NULL) {
    if(scale) *scale = 0;
    return 0;
  }

  int    tmpScale = 0;
  UINT64 result   = p->n;
  UINT   digits   = getDecimalDigitCount(p->n), firstDigits = digits;
  if(digits >= k) {
    result /= pow10(digits-k); // digits-k <= BIGREAL_LOG10BASE, so pow10 will not fail
    if(scale) {
      while(result % 10 == 0) {
        result /= 10;
        tmpScale++;
      }
    }
  } else { // digits < k
    if(scale) {
      for(p = p->next; digits < k; digits += BIGREAL_LOG10BASE) {
        if(p) {
          const BRDigitType p10 = pow10(min(BIGREAL_LOG10BASE,k-digits));
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
      for(p = p->next; digits < k; digits += BIGREAL_LOG10BASE) {
        const BRDigitType p10 = pow10(min(BIGREAL_LOG10BASE,k-digits));
        result *= p10;
        if(p) {
          result += p->n / (BIGREALBASE/p10);
          p = p->next;
        }
      }
    }
  }

  if(scale) {
    *scale  = m_expo * BIGREAL_LOG10BASE + firstDigits - 1 + tmpScale - k;
  }
  return result;
}

//#define TRACE_QUOTREMAINDER

void quotRemainder64(const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder) {
  BigReal::validateQuotRemainderArguments(__TFUNCTION__, x, y, quotient, remainder);
  if(!BigReal::checkIsNormalQuotient(x, y, quotient, remainder)) return;

  switch(BigReal::compareAbs(x, y)) {
  case -1:
    if(remainder) *remainder = x;
    if(quotient)  quotient->setToZero();
    return;
  case 0:
    if(remainder) remainder->setToZero();
    if(quotient)  *quotient = quotient->getDigitPool()->_1();
    return;
  }

  // x != 0 && y != 0 && |x| > |y|
  if(BigReal::isPow10(y)) {
    BigReal tmp(x);
    const BRExpoType yp10 = BigReal::getExpo10(y);
    tmp.multPow10(-yp10,true);
    tmp.fractionate(quotient, remainder);
    if(remainder) remainder->multPow10(yp10,true);
    if(quotient)  quotient->setPositive();
    return;
  }

  DigitPool *pool = x.getDigitPool();

  BigReal z(x, pool);
  z.clrFlags(BR_NEG);

  BRExpoType   scale;
  const UINT64 yFirst       = y.getFirst64(MAXDIGITS_DIVISOR64,&scale);
  const int    yDigits      = BigReal::getDecimalDigitCount(yFirst);

  BigReal q(pool), t(pool), tmp(pool);
  while(BigReal::compareAbs(z, y) >= 0) {
    t.approxQuot64Abs(z, yFirst, scale).copySign(z);
    q += t;
    z -= BigReal::product(tmp, t, y, pool->_0(),0).copySign(t);
  }
  const bool yNeg = y.isNegative();
  if(!z.isNegative()) {
    if(isInteger(q)) {
      if(quotient)  *quotient  = (BigInt&)q;
      if(remainder) *remainder = z;
    } else {
      q.fractionate(quotient, &t);
      z += (t * y).copySign(z);
      if(BigReal::compareAbs(z, y) >= 0) {
        if(quotient)  ++(*quotient);
        if(remainder) *remainder = yNeg ? (z + y) : (z - y);
      } else {
        if(remainder) *remainder = z;
      }
    }
  } else { // z < 0
    if(isInteger(q)) {
      if(quotient)  {
        *quotient = (BigInt&)q;
        --(*quotient);
      }
      if(remainder) {
        *remainder = yNeg ? z - y : z + y;
      }
    } else {
      q.fractionate(quotient, &t);
      if(remainder == NULL) { // quotient != NULL
        if(BigReal::compareAbs(z, t * y) > 0) {
          --(*quotient);
        }
      } else { // remainder != NULL. quotient might be NULL
        *remainder = z + (t * y).copySign(t);
        if(remainder->isNegative()) {
          if(yNeg) *remainder -= y; else *remainder += y;
          if(quotient) {
            --(*quotient);
          }
        }
      }
    }
  }

  if(remainder && remainder->_isnormal()) {
    remainder->copySign(x); // sign(x % y) = sign(x), equivalent to built-in % operator
  }
}

BigReal modulusOperator64(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal remainder(pool);
  quotRemainder64(x,y, NULL, &remainder);
  return remainder;
}
