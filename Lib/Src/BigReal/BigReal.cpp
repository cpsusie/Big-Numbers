#include "pch.h"

void BigReal::insertDigit(BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *p = newDigit();
  p->n = n;
  p->next = m_first;
  p->prev = NULL;
  if(m_last == NULL) {
    m_last = p;
  } else {
    m_first->prev = p;
  }
  m_first = p;
}

// Assume *this != zero. ie m_first != NULL (and m_last != NULL)
void BigReal::insertZeroDigits(size_t count) {
  Digit *p;
  for(p = m_first; count--;) {
    Digit *q = newDigit();
    q->n    = 0;
    q->next = p;
    p->prev = q;
    p       = q;
  }
  (m_first = p)->prev = NULL;
}


void BigReal::insertAfter(Digit *p, BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *q = newDigit();
  q->n = n;
  q->prev = p;
  q->next = p->next;
  if(p->next) {
    p->next->prev = q;
  } else {
    m_last = q;
  }
  p->next = q;
}

void BigReal::insertZeroDigitsAfter(Digit *p, size_t count) {
  Digit *q = p->next;
  if(q) {                     // p has a tail => p != last
    while(count--) {          // Insert count digits just after p, making the new chain grow at the HEAD (=q)
      Digit *r = newDigit();
      r->n     = 0;
      r->next  = q;
      q->prev  = r;
      q        = r;
    }
    (p->next = q)->prev = p;  // No need to modify m_last. p has a tail, so it is not m_last
  } else {                    // q == NULL => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = 0;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = NULL;
  }
}

void BigReal::insertBorrowDigitsAfter(Digit *p, size_t count) {
  Digit *q = p->next;
  if(q) {                     // p has a tail => p != last
    while(count--) {          // Insert count digits just after p, making the new chain grow at the HEAD (=q)
      Digit *r = newDigit();
      r->n     = BIGREALBASE - 1;
      r->next  = q;
      q->prev  = r;
      q        = r;
    }
    (p->next = q)->prev = p;  // No need to modify m_last. p has a tail, so it is not m_last
  } else {                    // q == NULL => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = BIGREALBASE - 1;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = NULL;
  }
}

void BigReal::trimHead() {
  m_expo--;
  Digit *p;
  for(p = m_first->next; p && (p->n == 0); p = p->next, m_expo--);

  if(p == NULL) { // all digits were 0 => *this = zero
    setToZero();
  } else {
    deleteDigits(m_first, p->prev);
    (m_first = p)->prev = NULL;

    if(m_last->n == 0) {
      for(m_low++, p = m_last->prev; p->n == 0; p = p->prev, m_low++);
      deleteDigits(p->next, m_last);
      (m_last = p)->next = NULL;
    }
  }
}

void BigReal::trimTail() {
  m_low++;
  Digit *p;
  for(p = m_last->prev; p->n == 0; p = p->prev, m_low++);
  deleteDigits(p->next, m_last); // we know that there is at least one digit != 0 => p != NULL
  (m_last = p)->next = NULL;
}

BigReal &BigReal::copyDigits(const BigReal &src, size_t length) {
  assert(src._isnormal() && (m_first == NULL) && (m_last == NULL) && length <= src.getLength());
  if(length--) {
    const Digit *sd = src.m_first;
    (m_first = newDigit())->prev = NULL;
    Digit *p = m_first;
    p->n = sd->n;
    Digit *dd;
    for(sd = sd->next; length--; sd = sd->next, p = dd) {
      (dd = newDigit())->prev = p;
      dd->n   = sd->n;
      p->next = dd;
    }
    (m_last = p)->next = NULL;
  }
  return *this;
}

// Assume !_inormal() && src._isnormal()
BigReal &BigReal::copyAllDigits(const BigReal &src) {
  assert(!_isnormal() && src._isnormal());
  const Digit *sd = src.m_first;
  Digit       *dd, *p;
  (m_first = p = newDigit())->prev = NULL;
  p->n = sd->n;
  for(; sd = sd->next; p = dd) {
    (dd = newDigit())->prev = p;
    dd->n   = sd->n;
    p->next = dd;
  }
  (m_last = p)->next = NULL;
  return *this;
}

BigReal &BigReal::multPow10(BRExpoType exp, bool force) {
  if(!force) CHECKISMUTABLE(*this);
  if((exp == 0) || !_isnormal()) {
    return *this;
  }
  int        m = exp % BIGREAL_LOG10BASE;
  BRExpoType n = exp / BIGREAL_LOG10BASE;
  if(m == 0) {
    m_expo += n;
    m_low  += n;
    if(m_expo > BIGREAL_MAXEXPO || m_expo < BIGREAL_MINEXPO) {
      throwBigRealInvalidArgumentException(__TFUNCTION__, _T("Invalid m_expo:%s"), format1000(m_expo).cstr());
    }
  } else {
    if(m < 0) {
      m = -m;
    }
    const BRDigitType s = pow10(m);
    const BRDigitType t = BIGREALBASE / s;
    if(exp > 0) { // shift left
      Digit *p = m_first;
      Digit *q = p->next; // *this != 0 => p != NULL
      if(p->n >= t) {
        insertDigit(p->n / t);
        m_expo++;
      }
      while(q) {
        p->n = q->n/t + (p->n%t) * s;
        p = q;
        q = q->next;
      }
      p->n = (p->n%t) * s;
    } else { // exp < 0. shift right
      Digit *p = m_last;
      Digit *q = p->prev; // *this != 0 => p != NULL
      appendDigit((p->n % s) * t);
      m_low--;
      while(q) {
        p->n = p->n/s + (q->n%s) * t;
        p = q;
        q = q->prev;
      }
      p->n /= s;
    }
    m_expo += n;
    m_low  += n;
    trimZeroes();
  }
  return *this;
}

BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f) {
  assert(f._isfinite());
  CHECKISMUTABLE(to);
  if(&to == &from) return to;
  if(!f.isPositive() || !from._isnormal()) {
    to = from;
  } else if(f.m_expo < from.m_low) {
    to = from;
  } else if(f.m_expo > from.m_expo) {
    to = to.getDigitPool()->_0();
  } else { // from.m_low <= f.m_expo <= from.m_expo
    to.clearDigits();
    to.m_expo     = from.m_expo;
    to.m_low      = max(from.m_low, f.m_expo);
    COPYSIGN(to, from).copyDigits(from, to.m_expo - to.m_low + 1).trimZeroes();
  }
  return to;
}

BigReal &copy(BigReal &to, const BigReal &from, size_t length) {
  CHECKISMUTABLE(to);
  if(&to == &from) return to;
  if(!from._isnormal()) {
    to.setToNonNormal(from.m_low, from.isNegative());
  } else {
    length = minMax(length, (size_t)1, from.getLength());
    to.m_low = (to.m_expo = from.m_expo) - length + 1;
    COPYSIGN(to, from).clearDigits().copyDigits(from, length).trimZeroes();
  }
  return to;
}

int BigReal::compare(const BigReal &x, const BigReal &y) { // static
  assert(x._isfinite() && y._isfinite());
  if(&x == &y) {
    return 0;
  }
  const BRExpoType ce = x.m_expo - y.m_expo;
  if(ce) {
    if(!x._isnormal()) return y.isNegative() ?  1:-1;  // x==0, y is normal and not zero
    if(!y._isnormal()) return x.isNegative() ? -1: 1;  // y==0, x is normal and not zero
    switch((ordinal(x.isNegative())<<1) | ordinal(y.isNegative())) {
    case 0 : return sign(ce);  // x>0, y>0
    case 1 : return 1;         // x>0, y<0
    case 2 : return -1;        // x<0, y>0
    case 3 : return -sign(ce); // x<0, y<0
    default: NODEFAULT;
    }
  }
  // same expo
  if(!x._isnormal()) { // x==0,y==0
    return 0;
  }

  const int cs = ordinal(y.isNegative()) - ordinal(x.isNegative());
  if(cs) return cs; // different sign
  // same sign, same expo. Compare digits
  const Digit *xp, *yp;
  BRDigitDiffType ddiff;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    if((ddiff = (BRDigitDiffType)xp->n - (BRDigitDiffType)yp->n) != 0) {
      return x.isNegative() ? -sign(ddiff) : sign(ddiff);
    }
  }
  // same sign, same expo. same head. Compare length
  return xp ? (x.isNegative()?-1:1) : yp ? (x.isNegative()?1:-1) : 0;
}

int BigReal::compareAbs(const BigReal &x, const BigReal &y) { // static
  assert(x._isfinite() && y._isfinite());
  if(&x == &y) {
    return 0;
  }
  const BRExpoType c = x.m_expo - y.m_expo;
  if(c) {
    if(!x._isnormal()) return -1;  // y is normal and not zero => x < y
    if(!y._isnormal()) return  1;  // x is normal and not zero => x > y;
    return sign(c); // both are normal and not zero, but different expo
  }
  // x.m_expo == y.m_expo
  if(!x._isnormal()) { // both are zero
    return 0;
  }

  // Compare digits
  const Digit *xp, *yp;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    const BRDigitDiffType d = (BRDigitDiffType)xp->n - (BRDigitDiffType)yp->n;
    if(d) {
      return sign(d);
    }
  }
  return xp ? 1 : yp ? -1 : 0;
}

#define CHECKISNORMAL(x)                                      \
if(!x._isnormal()) {                                          \
  if(x.isZero()) {                                            \
    return 0;                                                 \
  } else {                                                    \
    throwBigRealGetIntegralTypeUndefinedException(method, x); \
  }                                                           \
}

int  isInt(const BigReal &v) {
  return isInteger(v) && (v >= BigReal::_i32_min) && (v <= BigReal::_i32_max);
}
UINT isUint(const BigReal &v) {
  return isInteger(v) && !v.isNegative() && (v <= BigReal::_ui32_max);
}
bool isInt64(const BigReal &v) {
  return isInteger(v) && (v >= BigReal::_i64_min) && (v <= BigReal::_i64_max);
}
bool isUint64(const BigReal &v) {
  return isInteger(v) && !v.isNegative() && (v <= BigReal::_ui64_max);
}
bool isInt128(const BigReal &v) {
  return isInteger(v) && (v >= BigReal::_i128_min) && (v <= BigReal::_i128_max);
}
bool isUint128(const BigReal &v) {
  return isInteger(v) && !v.isNegative() && (v <= BigReal::_ui128_max);
}

long getLong(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v > BigReal::_i32_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i32_max));
  }
  if(v < BigReal::_i32_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i32_min));
  }

  intptr_t   result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (long)(v.isNegative() ? -result : result);
}

ULONG getUlong(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
  }
  if(v > BigReal::_ui32_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui32_max));
  }

  size_t     result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (ULONG)result;
}

INT64 getInt64(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v > BigReal::_i64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i64_max));
  }
  if(v < BigReal::_i64_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i64_min));
  }

  INT64      result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return v.isNegative() ? -result : result;
}

UINT64 getUint64(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
  }
  if(v > BigReal::_ui64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui64_max));
  }

  UINT64     result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}

_int128 getInt128(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v > BigReal::_i128_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i128_max));
  }
  if(v < BigReal::_i128_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i128_min));
  }

  _int128    result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return v.isNegative() ? -result : result;
}

_uint128 getUint128(const BigReal &v) {
  DEFINEMETHODNAME;
  CHECKISNORMAL(v)
  if(v.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
  }
  if(v > BigReal::_ui128_max) {
    throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui128_max));
  }

  _uint128   result = 0;
  BRExpoType i      = v.m_expo;
  for(const Digit *p = v.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}

ULONG BigReal::hashCode() const {
  size_t s;
  switch(_fpclass(*this)) {
  case _FPCLASS_QNAN  : return 0xffffffff;
  case _FPCLASS_NINF  : return 0xfffffffe;
  case _FPCLASS_PZ    : return 0;
  case _FPCLASS_PINF  : return 0xfffffffd;
  case _FPCLASS_NN    : s = ~m_expo; break;
  case _FPCLASS_PN    : s = m_expo ; break;
  }

  for(const Digit *p = m_first; p; p = p->next) {
    s = s * 17 + p->n;
  }
  return sizetHash(s);
}

BigReal &BigReal::multiply2() {
  CHECKISMUTABLE(*this);
  bool carry = false;
  if(_isnormal()) {
    for(Digit *d = m_last; d; d = d->prev) {
      d->n *= 2;
      if(carry) d->n++;
      if(carry = (d->n >= BIGREALBASE)) {
        d->n -= BIGREALBASE;
      }
    }
    if(carry) {
      insertDigit(1);
      m_expo++;
    }
    trimZeroes();
  }
  return *this;
}

#define _BR2 (BIGREALBASE / 2)
BigReal &BigReal::divide2() {
  CHECKISMUTABLE(*this);
  bool borrow = false;
  if(_isnormal()) {
    for(Digit *d = m_first; d; d = d->next) {
      const bool nb = (d->n & 1) != 0;
      d->n /= 2;
      if(borrow) d->n += _BR2;
      borrow = nb;
    }
    if(borrow) {
      appendDigit(_BR2);
    } else {
      trimZeroes();
    }
  }
  return *this;
}
