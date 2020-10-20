#include "pch.h"

void BigReal::insertDigit(BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *p = newDigit();
  p->n = n;
  p->next = m_first;
  p->prev = nullptr;
  if(m_last == nullptr) {
    m_last = p;
  } else {
    m_first->prev = p;
  }
  m_first = p;
}

// Assume _isnormal() && (count > 0). ie m_first != nullptr (and m_last != nullptr)
void BigReal::insertZeroDigits(size_t count) {
  assert((count > 0) && !_isnormal());
#if defined(USE_FETCHDIGITLIST)
  if(count == 1) {
    Digit *p = newDigit();
    p->n     = 0;
    (p->next = m_first)->prev = p;
    (m_first = p)->prev = nullptr;
  } else {
    Digit *head = m_digitPool.fetchDigitList(count), *last = head->prev;
    last->next = nullptr;
    for(Digit *p = head;;) {
      p->n = 0;
      if(!(p = p->next)) {
        (last->next = m_first)->prev = last;
        (m_first = head)->prev = nullptr;
        break;
      }
    }
  }
#else
  Digit *p;
  for(p = m_first; count--;) {
    Digit *q = newDigit();
    q->n    = 0;
    q->next = p;
    p->prev = q;
    p       = q;
  }
  (m_first = p)->prev = nullptr;
#endif // USE_FETCHDIGITLIST
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

// Assume (count > 0) && p is a digit in digit-list of this.
void BigReal::insertZeroDigitsAfter(Digit *p, size_t count) {
  assert((count > 0) && (p != nullptr));
#if defined(USE_FETCHDIGITLIST)
  Digit *head = m_digitPool.fetchDigitList(count, 0), *last = head->prev, *q = p->next;
  if(last->next = q) q->prev = last; else m_last = last;
  (p->next = head)->prev = p;
#else // old style
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
  } else {                    // q == nullptr => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = 0;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = nullptr;
  }
#endif // USE_FETCHDIGITLIST
}

// Assume (count > 0) && p is a digit in digit-list of this.
void BigReal::insertBorrowDigitsAfter(Digit *p, size_t count) {
  assert((count > 0) && (p != nullptr));
#if defined(USE_FETCHDIGITLIST)
  Digit *head = m_digitPool.fetchDigitList(count, BIGREALBASE - 1), *last = head->prev, *q = p->next;
  if(last->next = q) q->prev = last; else m_last = last;
  (p->next = head)->prev = p;
#else // old style
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
  } else {                    // q == nullptr => p == m_last (and maybe m_first too)
    for(q = p; count--;) {    // Append count digits after each other, making the new chain grow at the END (=q)
      Digit *r = newDigit();
      r->n     = BIGREALBASE - 1;
      r->prev  = q;
      q->next  = r;
      q        = r;
    }
    (m_last = q)->next = nullptr;
  }
#endif // USE_FETCHDIGITLIST
}

void BigReal::trimHead() {
  int count = 1;

  Digit *p;
  for(p = m_first->next; p && (p->n == 0); p = p->next) count++;

  if(p == nullptr) { // all digits were 0 => *this = zero
    setToZero();
  } else {
    deleteDigits(m_first, p->prev);
    (m_first = p)->prev = nullptr;
    m_expo -= count;

    if(m_last->n == 0) {
      count = 1;
      for(p = m_last->prev; p->n == 0; p = p->prev) count++;
      deleteDigits(p->next, m_last);
      (m_last = p)->next = nullptr;
      m_low += count;
    }
  }
}

void BigReal::trimTail() {
  int count = 1;
  Digit *p;
  for(p = m_last->prev; p->n == 0; p = p->prev) count++;
  deleteDigits(p->next, m_last); // we know that there is at least one digit != 0 => p != nullptr
  (m_last = p)->next = nullptr;
  m_low += count;
}

BigReal &BigReal::copyDigits(const BigReal &src, size_t length) {
  assert(src._isnormal() && (m_first == nullptr) && (m_last == nullptr) && (length <= src.getLength()) && (length > 0));
#if defined(USE_FETCHDIGITLIST)
  Digit *dd = m_digitPool.fetchDigitList(length);
  (m_last   = dd->prev)->next = nullptr;
  (m_first  = dd)->prev = nullptr;
  for(const Digit *sd = src.m_first;;) {
    dd->n = sd->n;
    if(dd = dd->next) {
      sd = sd->next;
    } else {
      break;
    }
  }
#else
  if(length--) {
    const Digit *sd = src.m_first;
    (m_first = newDigit())->prev = nullptr;
    Digit *p = m_first;
    p->n = sd->n;
    Digit *dd;
    for(sd = sd->next; length--; sd = sd->next, p = dd) {
      (dd = newDigit())->prev = p;
      dd->n   = sd->n;
      p->next = dd;
    }
    (m_last = p)->next = nullptr;
  }
#endif // USE_FETCHDIGITLIST
  return *this;
}

// Assume !_inormal() && src._isnormal()
BigReal &BigReal::copyAllDigits(const BigReal &src) {
  assert(!_isnormal() && src._isnormal());
#if defined(USE_FETCHDIGITLIST)
  Digit *dd = m_digitPool.fetchDigitList(src.getLength());
  (m_last   = dd->prev)->next = nullptr;
  (m_first  = dd)->prev = nullptr;
  for(const Digit *sd = src.m_first;;) {
    dd->n = sd->n;
    if(sd = sd->next) {
      dd = dd->next;
    } else {
      break;
    }
  }
#else
  const Digit *sd = src.m_first;
  Digit       *dd, *p;
  (m_first = p = newDigit())->prev = nullptr;
  p->n = sd->n;
  for(; sd = sd->next; p = dd) {
    (dd = newDigit())->prev = p;
    dd->n   = sd->n;
    p->next = dd;
  }
  (m_last = p)->next = nullptr;
#endif // USE_FETCHDIGITLIST
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
      Digit *q = p->next; // *this != 0 => p != nullptr
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
      Digit *q = p->prev; // *this != 0 => p != nullptr
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
    to.setToZero();
  } else { // from.m_low <= f.m_expo <= from.m_expo
    to.clearDigits();
    to.m_expo     = from.m_expo;
    to.m_low      = max(from.m_low, f.m_expo);
    to.copyDigits(from, to.m_expo - to.m_low + 1).copySign(from).trimZeroes();
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
    to.clearDigits().copyDigits(from, length).copySign(from).trimZeroes();
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

#define CHECKISNORMAL() if(!_isnormal()) return 0

bool isInt(const BigReal &v, int *n) {
  if(isInteger(v) && (v >= BigReal::_i32_min) && (v <= BigReal::_i32_max)) {
    if(n) *n = (int)v;
    return true;
  }
  return false;
}
bool isUint(const BigReal &v, UINT *n) {
  if(isInteger(v) && !v.isNegative() && (v <= BigReal::_ui32_max)) {
    if(n) *n = (UINT)v;
    return true;
  }
  return false;
}
bool isInt64(const BigReal &v, INT64 *n) {
  if(isInteger(v) && (v >= BigReal::_i64_min) && (v <= BigReal::_i64_max)) {
    if(n) *n = (INT64)v;
    return true;
  }
  return false;
}
bool isUint64(const BigReal &v, UINT64 *n) {
  if(isInteger(v) && !v.isNegative() && (v <= BigReal::_ui64_max)) {
    if(n) *n = (UINT64)v;
    return true;
  }
  return false;
}
bool isInt128(const BigReal &v, _int128 *n) {
  if(isInteger(v) && (v >= BigReal::_i128_min) && (v <= BigReal::_i128_max)) {
    if(n) *n = (_int128)v;
    return true;
  }
  return false;
}
bool isUint128(const BigReal &v, _uint128 *n) {
  if(isInteger(v) && !v.isNegative() && (v <= BigReal::_ui128_max)) {
    if(n) *n = (_uint128)v;
    return true;
  }
  return false;
}

#define ITYPE2EXPO(type,s) max((((int)sizeof(type)*8-(s))*301/1000+1) / (BIGREAL_LOG10BASE),2)

// s=0 if unsigned type, 1 for signed
// Calculate max(ceil(log10(type_max))/log10(base)),2)
// log10(type_max) = log2(type_max)*log10(2)
// LOG10BASE        2   4   6   8  18
// int      (s=1):  5   2   2   2   2
// UINT     (s=0):  5   2   2   2   2
// INT64    (s=1):  9   4   3   2   2
// UINT64   (s=0): 10   5   3   2   2
// _int128  (s=1): 19   9   6   4   2
// _uint128 (s=0): 19   9   6   4   2

BigReal::operator int() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v > BigReal::_i32_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i32_max));
    }
    if(v < BigReal::_i32_min) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i32_min));
    }
  }
*/
  intptr_t   result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(int,1));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (int)(isNegative() ? -result : result);
}

BigReal::operator unsigned int() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v.isNegative()) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
    }
    if(v > BigReal::_ui32_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui32_max));
    }
  }
*/
  size_t     result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(unsigned int,0));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (UINT)result;
}

BigReal::operator __int64() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v > BigReal::_i64_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i64_max));
    }
    if(v < BigReal::_i64_min) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i64_min));
    }
  }
*/
  INT64      result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(INT64,1));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return isNegative() ? -result : result;
}

BigReal::operator unsigned __int64() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v.isNegative()) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
    }
    if(v > BigReal::_ui64_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui64_max));
    }
  }
*/
  UINT64     result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(UINT64, 0));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}

BigReal::operator _int128() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v > BigReal::_i128_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_i128_max));
    }
    if(v < BigReal::_i128_min) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, toString(BigReal::_i128_min));
    }
  }
*/
  _int128    result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(_int128, 1));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return isNegative() ? -result : result;
}

BigReal::operator _uint128() const {
  DEFINEMETHODNAME;
  CHECKISNORMAL();
/*
  if(validate) {
    if(v.isNegative()) {
      throwBigRealGetIntegralTypeUnderflowException(method, v, _T("0"));
    }
    if(v > BigReal::_ui128_max) {
      throwBigRealGetIntegralTypeOverflowException(method, v, toString(BigReal::_ui128_max));
    }
  }
*/
  _uint128   result = 0;
  BRExpoType i      = min(m_expo, ITYPE2EXPO(_uint128, 0));
  for(const Digit *p = m_first; p && (i-- >= 0); p = p->next) {
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
      m_low--;
    }
    trimZeroes();
  }
  return *this;
}
