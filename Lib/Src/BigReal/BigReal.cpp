#include "pch.h"

DEFINECLASSNAME(BigReal);

void BigReal::insertAfter(Digit *q, BRDigitType n) {
//  assert(n < BIGREALBASE);
  Digit *p = newDigit();
  p->n = n;
  p->prev = q;
  p->next = q->next;
  if(q->next) {
    q->next->prev = p;
  } else {
    m_last = p;
  }
  q->next = p;
}

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

//#define COUNT_CALLS

#include <CallCounter.h>

#ifdef COUNT_CALLS
#define DECLARE_CALLCOUNTER(name) static CallCounter _callCounter(name);
#define COUNTKEYCALL(n)                              _callCounter.incr(n)
#define COUNTCALL()                                  _callCounter.incr()
#else
#define DECLARE_CALLCOUNTER(name)
#define COUNTKEYCALL(n)
#define COUNTCALL()
#endif

// Assume *this != zero. ie m_first != NULL (and m_last != NULL)
void BigReal::insertZeroDigits(size_t count) { 
  DECLARE_CALLCOUNTER("insertZeroDigits");

  COUNTKEYCALL(count);

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


void BigReal::insertZeroDigitsAfter(Digit *p, size_t count) {
  DECLARE_CALLCOUNTER("insertZeroDigitsAfter");

  COUNTKEYCALL(count);

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
  DECLARE_CALLCOUNTER("insertBorrowDigitsAfter");

  COUNTKEYCALL(count);

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

void BigReal::trimHead() { // assume m_first && m_first == 0
  m_expo--;
  Digit *p;
  for(p = m_first->next; p && (p->n == 0); p = p->next, m_expo--);

  if(p == NULL) { // all digits were 0 => *this = zero
    setToZero();
  } else {
    deleteDigits(m_first, p->prev);
    (m_first = p)->prev = NULL;

    if(m_last->n == 0) {
      for(m_low++, p = m_last->prev; p && (p->n == 0); p = p->prev, m_low++);
      deleteDigits(p->next, m_last);
      (m_last = p)->next = NULL;
    }
  }
}

void BigReal::trimTail() { // assume m_first != NULL => m_last != NULL and m_last->n == 0
  m_low++;
  Digit *p;
  for(p = m_last->prev; p && (p->n == 0); p = p->prev, m_low++);
  deleteDigits(p->next, m_last); // we know that there is at least one digit != 0 => p != NULL
  (m_last = p)->next = NULL;
}

// Assume src != zero && length <= src.getLength() && m_first == m_last == NULL
void BigReal::copyDigits(const BigReal &src, size_t length) {
  DECLARE_CALLCOUNTER("copyDigits");

  COUNTKEYCALL(length);

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
      ;
    }
    (m_last = p)->next = NULL;
  } else {
    m_first = m_last = NULL;
  }
}

// Assume src != zero && m_first == m_last == NULL
void BigReal::copyAllDigits(const BigReal &src) {
  DECLARE_CALLCOUNTER("copyAllDigits");

  COUNTCALL();

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
}

// Assume n = [0..BIGREALBASE[
int BigReal::getDecimalDigitCount32(unsigned long n) { // static
  // kind of binary search
  if(n < 10000) {
    if(n < 100) {
      if(n < 10) {
        return n ? 1 : 0;
      } else { // 10 <= n < 1e2
        return 2;
      }
    } else { // 1e2 <= n < 1e4
      return n < 1000 ? 3 : 4;
    }
  } else { // 1e4 <= n < 1e9
    if(n < 1000000) {
      return n < 100000 ? 5 : 6;
    } else { // 1e6 <= n < 1e9
      return n < 10000000 ? 7 : 8;
    }
  }
}

// Assume n = [0..1eMAXDIGITS_INT64]
int BigReal::getDecimalDigitCount64(unsigned __int64 n) { // static
  static const unsigned __int64 pow10Table[] = {
     1ui64
    ,10ui64
    ,100ui64
    ,1000ui64
    ,10000ui64
    ,100000ui64
    ,1000000ui64
    ,10000000ui64
    ,100000000ui64
    ,1000000000ui64
    ,10000000000ui64
    ,100000000000ui64
    ,1000000000000ui64
    ,10000000000000ui64
    ,100000000000000ui64
    ,1000000000000000ui64
    ,10000000000000000ui64
    ,100000000000000000ui64
    ,1000000000000000000ui64
    ,10000000000000000000ui64
  };
  int l = 0, r = ARRAYSIZE(pow10Table);
  while(l < r) {
    const int m = (l+r)/2;
    const unsigned __int64 &p10 = pow10Table[m];
    if(p10 <= n) {
      l = m + 1;
    } else {
      r = m;
    }
  }
  return r;
}

const BRDigitType BigReal::s_power10Table[POWER10TABLESIZE] = {
  1
 ,10
 ,100
 ,1000
 ,10000
 ,100000
 ,1000000
 ,10000000
 ,100000000
 ,1000000000
#ifdef IS64BIT
 ,10000000000
 ,100000000000
 ,1000000000000
 ,10000000000000
 ,100000000000000
 ,1000000000000000
 ,10000000000000000
 ,100000000000000000
 ,1000000000000000000
 ,10000000000000000000
#endif
};

// Return p if n = 10^p for p = [0..9]. else return -1.
int BigReal::isPow10(size_t n) { // static
  switch(n) {
  case 1         : return  0;
  case 10        : return  1;
  case 100       : return  2;
  case 1000      : return  3;
  case 10000     : return  4;
  case 100000    : return  5;
  case 1000000   : return  6;
  case 10000000  : return  7;
  case 100000000 : return  8;
  case 1000000000: return  9;
#ifdef IS64BIT
  case 10000000000         : return  10;
  case 100000000000        : return  11;
  case 1000000000000       : return  12;
  case 10000000000000      : return  13;
  case 100000000000000     : return  14;
  case 1000000000000000    : return  15;
  case 10000000000000000   : return  16;
  case 100000000000000000  : return  17;
  case 1000000000000000000 : return  18;
  case 10000000000000000000: return  19;
#endif
  default        : return -1;
  }
}

bool BigReal::isPow10(const BigReal &x) { // static
  return !x.isZero() && (x.getLength() == 1) && (isPow10(x.getFirstDigit()) >= 0);
}

BigReal &BigReal::multPow10(BRExpoType exp) {
  DEFINEMETHODNAME;
  if(isZero()) {
    return *this;
  }
  int m = exp % LOG10_BIGREALBASE;
  BRExpoType   n = exp / LOG10_BIGREALBASE;
  if(m == 0) {
    m_expo += n;
    m_low  += n;
    if(m_expo > BIGREAL_MAXEXPO || m_expo < BIGREAL_MINEXPO) {
      throwBigRealInvalidArgumentException(method, _T("Invalid m_expo:%s"), format1000(m_expo).cstr());
    }
  } else {
    if(m < 0) {
      m = -m;
    }
    const BRDigitType s = pow10(m);
    const BRDigitType t = BIGREALBASE / s;
    if(exp > 0) { // shift left
      Digit *p = m_first; 
      Digit *q = p->next; // *this != 0, =>. p != NULL
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
      Digit *q = p->prev; // *this != 0, =>. p != NULL
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
  SETBIGREALDEBUGSTRING(*this);
  return *this;
}


BigReal &copy(BigReal &to, const BigReal &from, const BigReal &f) {
  if(!f.isPositive() || from.isZero()) {
    to = from;
  } else if(f.m_expo < from.m_low) {
    to = from;
  } else if(f.m_expo > from.m_expo) {
    to = to.getDigitPool()->get0();
  } else { // from.m_low <= f.m_expo <= from.m_expo
    to.clearDigits();
    to.m_expo     = from.m_expo;
    to.m_low      = max(from.m_low, f.m_expo);
    to.m_negative = from.m_negative;
    to.copyDigits(from, to.m_expo - to.m_low + 1);
    to.trimZeroes();
    SETBIGREALDEBUGSTRING(to);
  }
  return to;
}

BigReal &copy(BigReal &to, const BigReal &from, unsigned int length) {
  if(from.isZero()) {
    to.setToZero();
  } else {
    length = minMax(length, (unsigned int)1, (unsigned int)from.getLength());
    to.m_low = (to.m_expo = from.m_expo) - length + 1;
    to.m_negative = from.m_negative;
    to.clearDigits();
    to.copyDigits(from, length);
    to.trimZeroes();
    SETBIGREALDEBUGSTRING(to);
  }
  return to;
}

int compare(const BigReal &x, const BigReal &y) {
  if(&x == &y) {
    return 0;
  }
  if(x.isZero()) {
    return y.isZero() ? 0 : y.m_negative ? 1 : -1;
  }

  // x != 0
  if(y.isZero()) {
    return x.m_negative ? -1 : 1;
  }

  // x != 0 && y != 0
  if(x.m_negative != y.m_negative) {
    return x.m_negative ? -1 : 1;
  }

  // sign(x) == sign(y) 
  if(x.m_expo > y.m_expo) { // abs(x) > abs(y)
    return x.m_negative ? -1 : 1;
  }
  if(x.m_expo < y.m_expo) { // abs(x) < abs(y)
    return x.m_negative ? 1 : -1;
  }

  // Compare digits
  const int s = x.m_negative ? -1 : 1;
  const Digit *xp, *yp;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    const long d = long(xp->n - yp->n);
    if(d) {
      return sign(d) * s;
    }
  }
  return xp ? s : yp ? -s : 0;
}

int compareAbs(const BigReal &x, const BigReal &y) {
  if(&x == &y) {
    return 0;
  }
  if(x.isZero()) {
    return y.isZero() ? 0 : -1;
  }

  // x != 0
  if(y.isZero()) {
    return 1;
  }

  // x != 0 && y != 0
  if(x.m_expo > y.m_expo) { // abs(x) > abs(y)
    return 1;
  }
  if(x.m_expo < y.m_expo) { // abs(x) < abs(y)
    return -1;
  }

  // Compare digits
  const Digit *xp, *yp;
  for(xp = x.m_first, yp = y.m_first; xp && yp; xp = xp->next, yp = yp->next) {
    const long d = long(xp->n - yp->n);
    if(d) {
      return sign(d);
    }
  }
  return xp ? 1 : yp ? -1 : 0;
}

bool operator==(const BigReal &x,  const BigReal &y) {
  return compare(x,y) == 0;
}
  
bool operator!=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) != 0;
}
  
bool operator>=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) >= 0;
}
  
bool operator<=(const BigReal &x,  const BigReal &y) {
  return compare(x,y) <= 0;
}
  
bool operator>(const BigReal &x,  const BigReal &y) {
  return compare(x,y) >  0;
}
  
bool operator<(const BigReal &x,  const BigReal &y) {
  return compare(x,y) <  0;
}
  
int getInt(const BigReal &x) {
  return (int)getLong(x);
}

unsigned int getUint(const BigReal &x) {
  return (unsigned int)getUlong(x);
}

long getLong(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x > ConstBigReal::_long_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_long_max));
  }
  if(x < ConstBigReal::_long_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_long_min));
  }

  intptr_t   result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (long)(x.isNegative() ? -result : result);
}

unsigned long getUlong(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, _T("0"));
  }
  if(x > ConstBigReal::_ulong_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_ulong_max));
  }

  size_t     result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;

  return (unsigned long)result;
}

__int64 getInt64(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x > ConstBigReal::_i64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_i64_max));
  }
  if(x < ConstBigReal::_i64_min) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, toString(ConstBigReal::_i64_min));
  }

  __int64    result = 0;
  BRExpoType i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return x.isNegative() ? -result : result;
}

unsigned __int64 getUint64(const BigReal &x) {
  DEFINEMETHODNAME;
  if(x.isZero()) {
    return 0;
  }

  if(x.isNegative()) {
    throwBigRealGetIntegralTypeUnderflowException(method, x, _T("0"));
  }
  if(x > ConstBigReal::_ui64_max) {
    throwBigRealGetIntegralTypeOverflowException(method, x, toString(ConstBigReal::_ui64_max));
  }

  unsigned __int64 result = 0;
  BRExpoType       i      = x.m_expo;
  for(const Digit *p = x.m_first; p && (i-- >= 0); p = p->next) {
    result = result * BIGREALBASE + p->n;
  }
  for(;i-- >= 0;) result *= BIGREALBASE;
  return result;
}

unsigned long BigReal::hashCode() const {
  size_t s = m_expo;
  if(isZero()) {
    return 0;
  }
  if(m_negative) s = ~s;
  for(const Digit *p = m_first; p; p = p->next) {
    s = s * 17 + p->n;
  }
#ifdef IS32BIT
  return s;
#else // IS64BIT
  return ((unsigned long)(s >> 32)) ^ ((unsigned long)(s & 0xffffffff));
#endif
}

static void throwAssertionException(_In_z_ _Printf_format_string_ const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  throwBigRealException(_T("assertIsValidBigReal:%s"), msg.cstr());
}

void BigReal ::assertIsValidBigReal() const {
  if(m_expo == BIGREAL_ZEROEXPO) {
    if(m_first != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_ZEROEXPO, but m_first != NULL"));
    }
    if(m_last != NULL) {
      throwAssertionException(_T("m_expo == BIGREAL_ZEROEXPO, but m_last != NULL"));
    }
    return;
  }
  unsigned int digitCount = 0;
  for(const Digit *p = m_first; p; p = p->next) {
    if(p->n >= BIGREALBASE) {
      throwAssertionException(_T("Digit(%s) (=%s) >= BIGREALBASE (=%lu)")
                             ,format1000(digitCount).cstr()
                             ,format1000(p->n).cstr()
                             ,format1000(BIGREALBASE).cstr());
    }
    digitCount++;
  }
  if(digitCount == 0) {
    throwAssertionException(_T("#digits in chain = 0. x != 0"));
  }
  if(digitCount != (unsigned int)getLength()) {
    throwAssertionException(_T("#digits in chain (=%s) != getLength() (=%s)")
                           ,format1000(digitCount).cstr()
                           ,format1000(getLength()).cstr());
  }
  if(m_first->n == 0) {
    throwAssertionException(_T("m_first->n = 0"));
  }
  if(m_last->n == 0) {
    throwAssertionException(_T("m_last->n = 0"));
  }
  if(m_expo > BIGREAL_MAXEXPO) {
    throwAssertionException(_T("m_expo > BIGREAL_MAXEXPO (=%s)"), format1000(m_expo).cstr());
  }
  if(m_expo < BIGREAL_MINEXPO) {
    throwAssertionException(_T("m_expo < BIGREAL_MINEXPO (=%s)"), format1000(m_expo).cstr());
  }
}
