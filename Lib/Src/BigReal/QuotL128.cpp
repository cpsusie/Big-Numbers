#include "pch.h"

#if defined(IS64BIT)

#define MAXDIGITS_INT128     38
#define MAXDIGITS_DIVISOR128 ((MAXDIGITS_INT128+1)/2)

class QuotL128Constants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;
  const ConstBigReal c4;

  QuotL128Constants()
    :c1(e(BigReal(24),-2))
    ,c2(e(BigReal( 4),-2))
    ,c3(e(BigReal( 8),-2))
    ,c4(e(BigReal(11),-1))
  {
  }
};

static const QuotL128Constants Q128C;

// Assume x and y are both normal (finite and != 0) and f>0.
// x/y with |error| < f. School method. using class _int128 for division
BigReal BigReal::quotLinear128(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  assert(isNormalQuotient(x, y) && f.isPositive());

  const BigReal v = APCprod(<, APCprod(<, f, Q128C.c1, pool), APCabs(#,y,pool), pool); // v > 0

  BigReal z(pool);
  copy(z, x, v*Q128C.c2).clrFlags(BR_NEG);

  BigReal u(pool);
  u.approxQuot32(z, v);
  u.approxQuot32(v, getExpo10N(u)*Q128C.c3 + Q128C.c4);

  BRExpoType scale;
  _uint128 yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);

  BigReal result(pool), t(pool), tmp(pool);
  bool loopDone = false;
  for(; compareAbs(z,v) > 0; loopDone = true) {
    t.approxQuot128Abs(z, yFirst, scale).copySign(z);
    result += t;
    z -= product(tmp, t, y, u, 0).copySign(t);
  }

  if(loopDone) {
    return result.setSignByProductRule(x,y);
  } else {
    return result.approxQuot128Abs(x, yFirst, scale).setSignByProductRule(x,y);
  }
}

BigReal &BigReal::approxQuot128(const BigReal &x, const BigReal &y) {
  assert(y._isnormal());
  BRExpoType scale;
  _uint128 yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);
  return approxQuot128Abs(x, yFirst, scale);
}

BigReal &BigReal::approxQuot128Abs(const BigReal &x, const _uint128 &y, BRExpoType scale) {
  assert(y != 0);
  _uint128 xFirst;
  *this = x.getFirst128(xFirst, MAXDIGITS_INT128)/y;
  return multPow10(getExpo10(x) - scale - MAXDIGITS_INT128,true);
}

#define ADJUSTSCALEMOD10(i128)                      \
{ if(HI64(i128) == 0) {                             \
    for(; LO64(i128) % 10 == 0; LO64(i128) /= 10) { \
      tmpScale++;                                   \
    }                                               \
  } else {                                          \
    for(; i128 % 10 == 0; i128 /= 10) {             \
      tmpScale++;                                   \
    }                                               \
  }                                                 \
}

_uint128 &BigReal::getFirst128(_uint128 &dst, const UINT k, BRExpoType *scale) const {
  assert(k <= MAXDIGITS_INT128);
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
    dst /= pow10(digits-k); // digits-k <= BIGREAL_LOG10BASE, so pow10 will not fail
    if(scale) {
      ADJUSTSCALEMOD10(dst);
    }
  } else { // digits < k
    if(scale) {
      for(p = p->next; (UINT)digits < k; digits += BIGREAL_LOG10BASE) {
        if(p) {
          const BRDigitType p10 = pow10(min(BIGREAL_LOG10BASE,k-digits));
          dst = dst * p10 + p->n / (BIGREALBASE/p10);
          p = p->next;
        } else {
          tmpScale = k - digits;
          break;
        }
      }
      ADJUSTSCALEMOD10(dst);
    } else { // scale == NULL
      for(p = p->next; (UINT)digits < k; digits += BIGREAL_LOG10BASE) {
        const BRDigitType p10 = pow10(min(BIGREAL_LOG10BASE,k-digits));
        dst *= p10;
        if(p) {
          dst += p->n / (BIGREALBASE/p10);
          p = p->next;
        }
      }
    }
  }

  if(scale) {
    *scale  = m_expo * BIGREAL_LOG10BASE + firstDigits - 1 + tmpScale - k;
  }
  return dst;
}

void quotRemainder128(const BigReal &x, const BigReal &y, BigInt *quotient, BigReal *remainder) {
  BigReal::validateQuotRemainderArguments(__TFUNCTION__, x, y, quotient, remainder);
  if(!BigReal::checkIsNormalQuotient(x, y, quotient, remainder)) return;

  switch(BigReal::compareAbs(x, y)) {
  case -1:
    if(remainder) *remainder = x;
    if(quotient )  quotient->setToZero();
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
    if(quotient ) quotient->setPositive();
    return;
  }

  DigitPool *pool = x.getDigitPool();

  BigReal z(x, pool);
  z.clrFlags(BR_NEG);

  BRExpoType             scale;
  _uint128               yFirst;
  y.getFirst128(yFirst, MAXDIGITS_DIVISOR128, &scale);
  const int              yDigits      = BigReal::getDecimalDigitCount((UINT64)yFirst);

  BigReal q(pool), t(pool), tmp(pool);
  while(BigReal::compareAbs(z, y) >= 0) {
    t.approxQuot128Abs(z, yFirst, scale).copySign(z);
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
        if(quotient) { ++(*quotient); }
        if(remainder) { *remainder = yNeg ? (z + y) : (z - y); }
      } else {
        if(remainder) { *remainder = z; }
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
    remainder->copySign(x);
  }
}

BigReal modulusOperator128(const BigReal &x, const BigReal &y) {
  DigitPool *pool = x.getDigitPool();
  BigReal remainder(pool);
  quotRemainder128(x,y, NULL, &remainder);
  return remainder;
}

#endif // IS64BIT
