#include "pch.h"

#define MAXDIGITS_INT32     9
#define MAXDIGITS_DIVISOR32 ((MAXDIGITS_INT32+1)/2)

class QuotL32Constants {
public:
  const ConstBigReal c1;
  const ConstBigReal c2;
  const ConstBigReal c3;
  const ConstBigReal c4;

  QuotL32Constants()
    :c1(e(BigReal(24),-2))
    ,c2(e(BigReal( 4),-2))
    ,c3(e(BigReal( 8),-2))
    ,c4(e(BigReal(11),-1))
  {
  }
};

static const QuotL32Constants Q32C;

// Assume isNormalQuotient(x, y) and f>0.
// x/y with |error| < f. School method. using built-in 32-bit division
BigReal BigReal::quotLinear32(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *pool) { // static
  assert(isNormalQuotient(x, y) && f.isPositive());

  const BigReal v = APCprod(<, APCprod(<, f, Q32C.c1, pool), APCabs(#,y,pool), pool); // v >= 0

  BigReal z(pool);
  copy(z, x, v*Q32C.c2).clrFlags(BR_NEG);

  BigReal u(pool);
  u.approxQuot32(z, v);
  u.approxQuot32(v, getExpo10N(u)*Q32C.c3 + Q32C.c4);

  BRExpoType scale;
  const unsigned long yFirst = y.getFirst32(MAXDIGITS_DIVISOR32,&scale);

  BigReal result(pool), t(pool), tmp(pool);
  bool loopDone = false;
  for(; compareAbs(z,v) > 0; loopDone = true) {
    t.approxQuot32Abs(z, yFirst, scale).copySign(z);
    result += t;
    z -= product(tmp, t, y, u, 0).copySign(t);
  }

  if(loopDone) {
    return result.setSignByProductRule(x,y);
  } else {
    return result.approxQuot32Abs(x, yFirst, scale).setSignByProductRule(x,y);
  }
}

BigReal &BigReal::approxQuot32(const BigReal &x, const BigReal &y) {
  assert(y._isnormal());
  BRExpoType scale;
  const unsigned long yFirst = y.getFirst32(MAXDIGITS_DIVISOR32,&scale);
  return approxQuot32Abs(x, yFirst, scale).setSignByProductRule(x, y);
}

BigReal &BigReal::approxQuot32Abs(const BigReal &x, unsigned long y, BRExpoType scale) {
  assert(y != 0);
  const unsigned long q = x.getFirst32(MAXDIGITS_INT32)/y;
  *this = q;
  return multPow10(getExpo10(x) - scale - MAXDIGITS_INT32, true);
}

BigReal BigReal::reciprocal(const BigReal &x, DigitPool *digitPool) { // static
  static const int k = MAXDIGITS_DIVISOR32;
  BigReal result(pow10(MAXDIGITS_INT32)/x.getFirst32(k), digitPool?digitPool:x.getDigitPool());
  return result.multPow10(k - 1 - MAXDIGITS_INT32 - getExpo10(x),true).copySign(x);
}

// Assume k <= MAXDIGITS_INT32 (=9). scale can be nullptr.
// Return the first k decimal digits of this.
// if scale != nullptr, result = n/10^scale, where scale = max(s|(n/10^s) % 10 != 0) and n = first k decimal digits of this
// if scale == nullptr, result = first k decimal digits of this, which can contain trailing zeroes
unsigned long BigReal::getFirst32(const UINT k, BRExpoType *scale) const {
  assert(k <= MAXDIGITS_INT32);

  const Digit *p = m_first;
  if(p == nullptr) {
    if(scale) *scale = 0;
    return 0;
  }

  BRExpoType    tmpScale = 0;
  BRDigitType   result   = p->n;

  int           digits   = getDecimalDigitCount(result), firstDigits = digits;
  if((UINT)digits >= k) {
    result /= pow10(digits-k); // digits-k <= BIGREAL_LOG10BASE, so pow10 will not fail
    if(scale) {
      while(result % 10 == 0) {
        result /= 10;
        tmpScale++;
      }
    }
  } else { // digits < k
    if(scale) {
      for(p = p->next; (UINT)digits < k; digits += BIGREAL_LOG10BASE) {
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
    } else { // scale == nullptr
      for(p = p->next; (UINT)digits < k; digits += BIGREAL_LOG10BASE) {
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
    *scale = m_expo * BIGREAL_LOG10BASE + firstDigits - 1 + tmpScale - k;
  }
  return (unsigned long)result;
}
