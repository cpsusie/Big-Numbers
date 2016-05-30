#include "pch.h"

BigReal fabs(const BigReal &x) { // Absolute value of x (=|x|)
  if(!x.isNegative()) {
    return x;
  }
  BigReal result(x);
  result.setPositive();
  return result;
}

int BigReal::logBASE(double x) { // static
  return (int)(log10(x) / LOG10_BIGREALBASE);
}

int BigReal::getExpo2(const BigReal &x) { // static
  static const double log2_10 = 3.321928094887362; // = ln(10)/ln(2).
                                                   // so expo10(x) * log2_10 is approximately ln(x)/ln(10) * ln(10)/ln(2) 
                                                   // = ln(x)/ln(2) = ln2(x) approximately  expo2(x)
  if(x.isZero()) {
    return 0;
  }
  const int expo10 = getExpo10(x);
  return (expo10 > -4890) ? ((int)(log2_10 * expo10)) : ((int)(log2(x.getFirst32(9))) + (int)(log2_10 * (expo10-9)));
}

int BigReal::getDecimalDigits() const { // BigReal of decimal digits. 0 has length 1
  if(isZero()) {
    return 1;
  } else if(m_expo == m_low) {
    return getDecimalDigitCount(m_first->n);
  } else {
    int lastDigitCount = LOG10_BIGREALBASE;
    for(unsigned long last = m_last->n; last % 10 == 0;) {
      lastDigitCount--;
      last /= 10;
    }
    return (m_expo - m_low - 1) * LOG10_BIGREALBASE + getDecimalDigitCount(m_first->n) + lastDigitCount;
  }
}

BigReal e( const BigReal &x, int n, DigitPool *pool) { // x * pow(10,n)
  return BigReal(x,pool).multPow10(n);
}

bool even(const BigReal &x) {
  if(!isInteger(x)) {
    return false;
  }
  if(x.isZero()) {
    return true;
  }
  if(x.getLow() > 0) {
    return true; // x = integer * base^m_low
  }
  return (x.m_last->n & 1) == 0;
}

bool odd(const BigReal &x) {
  if(!isInteger(x)) {
    return false;
  }
  if(x.isZero()) {
    return false;
  }
  if(x.getLow() > 0) {
    return false; // x = integer * base^m_low;
  }
  return (x.m_last->n & 1) != 0;
}

BigInt floor(const BigReal &x) { // biggest integer <= x
  return BigInt(x);
}

BigInt ceil(const BigReal &x) { // smallest integer >= x
  DigitPool *pool = x.getDigitPool();
  if(x.isZero()) {
    return pool->get0();;
  } else if(x.m_expo < 0) { // |x| < 1
    return x.isNegative() ? pool->get0() : pool->get1();
  } else if(isInteger(x)) { // x is a BigInt
    return (BigInt)x;
  } else {  // Copy the integerpart of x.
    BigInt result(pool);
    result.copyDigits(x, (result.m_expo = x.m_expo)+1);
    result.m_low      = 0;
    result.m_negative = x.m_negative;
    result.trimZeroes();
    if(!result.m_negative) {
      ++result;
    }
    SETBIGREALDEBUGSTRING(result);
    return result;
  }
}

BigReal fraction(const BigReal &x) { // sign(x) * (|x| - floor(|x|))
  BigReal result;
  x.fractionate(NULL, &result);
  return result;
/*
  if(x.isZero()) {
    return x;
  }
  if(isInteger(x)) {
    return x.getDigitPool()->get0();
  }
  if(x.isNegative()) {
    const BigReal xp(fabs(x));
    return floor(xp) - xp;
  } else {
    return x - floor(x);
  }
*/
}

BigReal trunc(const BigReal &x, int prec) {  // sign(x) * ent(abs(x)*10^prec)*10^-prec
  if(x.isZero()) {
    return x;
  }
  BigReal result(x.getDigitPool());
  if(prec == 0) {
    result = (x.isNegative()) ? -floor(-x) : floor(x);
  } else {
    BigReal tmp(x);
    result = (x.isNegative()) ? -floor(-tmp.multPow10(prec)).multPow10(-prec) : floor(tmp.multPow10(prec)).multPow10(-prec);
  }
  return result;
}

BigReal round(const BigReal &x, int prec) { // sign(x) * ent(abs(x)*10^prec+0.5)*10^-prec
  if(x.isZero()) {
    return x;
  }
  DigitPool *pool = x.getDigitPool();
  BigReal result(pool);
  if(prec == 0) {
    result = (x.isNegative()) ? -floor(pool->getHalf() - x) : floor(x + pool->getHalf());
  } else {
    BigReal tmp(x);
    result = (x.isNegative()) ? -floor(pool->getHalf() - tmp.multPow10(prec)).multPow10(-prec) : floor(tmp.multPow10(prec) + pool->getHalf()).multPow10(-prec);
  }
  return result;
}

void BigReal::fractionate(BigInt *integerPart, BigReal *fractionPart) const {
  DEFINEMETHODNAME(fractionate);
  if(integerPart == fractionPart) {
    throwBigRealInvalidArgumentException(method, _T("integerPart is the same variable as fractionPart"));
  }
  if(isZero()) {
    if(integerPart ) integerPart->setToZero();
    if(fractionPart) fractionPart->setToZero();
    return;
  }
  if(isInteger(*this)) {
    if(integerPart ) *integerPart = *this;
    if(fractionPart) fractionPart->setToZero();
    return;
  }
  if(m_expo < 0) {
    if(integerPart ) integerPart->setToZero();
    if(fractionPart) *fractionPart = *this;
    return;
  }
  // x != 0 and digits on both sides of decimal-comma
  const Digit *sd = m_first;
  if(integerPart) {
    BigReal &intPart = *integerPart;
    intPart.m_negative = m_negative;
    intPart.m_expo     = m_expo;
    intPart.m_low      = 0;

    Digit *dd = intPart.m_first;
    int expo = m_expo;
    for(; dd && (expo-- >= 0); sd = sd->next, dd = dd->next) {
      dd->n = sd->n;
    }
    if(dd) {                        // remove axcess digits from integerPart
      Digit *saveLast = intPart.m_last;
      intPart.m_last = dd->prev;
      intPart.m_last->next = NULL;
      intPart.m_digitPool.deleteDigits(dd, saveLast);
    } else {
      for(;expo-- >= 0; sd = sd->next) { // still some digits to append to integerpart
        intPart.appendDigit(sd->n);
      }
    }
    intPart.trimZeroes();
    SETBIGREALDEBUGSTRING(*integerPart);
  } else { // Find first fractionPart, either from beginning or froam the end, depending on which is shortest
    if(m_expo <= -m_low) {
      for(int expo = m_expo; expo-- >= 0; sd = sd->next);
    } else {
      sd = m_last;
      for(int expo = m_low; ++expo < 0; sd = sd->prev);
    }
  }
  // Assume sd points to the first fraction digit
  if(fractionPart) {
    fractionPart->m_negative = m_negative;
    fractionPart->m_expo     = -1;
    fractionPart->m_low      = m_low;
    Digit *dd;
    for(dd = fractionPart->m_first; dd && sd; dd = dd->next, sd = sd->next) {
      dd->n = sd->n;
    }
    if(dd) { // remove excess digits from fractionPart
      Digit *saveLast = fractionPart->m_last;
      fractionPart->m_last = dd->prev;
      fractionPart->m_last->next = NULL;
      fractionPart->m_digitPool.deleteDigits(dd, saveLast);
    } else {
      for(; sd; sd = sd->next) {
        fractionPart->appendDigit(sd->n);
      }
    }
    fractionPart->trimZeroes();
    SETBIGREALDEBUGSTRING(*fractionPart);
  }
}

static inline unsigned long truncInt(unsigned long n, int prec) { // assume prec <= 0
  if(prec == 0) {
    return n;
  } else {
    const unsigned long p10 = BigReal::pow10(-prec);
    return (n / p10) * p10;
  }
}

static unsigned long roundInt(unsigned long n, int prec) { // assume prec <= 0
  if(prec == 0) {
    return n;
  } else {
    const unsigned long p10 = BigReal::pow10(-prec-1);
    unsigned long       v   = n / p10;
    const int           r   = v % 10;
    if(r >= 5) v += 10 - r; else v -= r;
    return v * p10;
  }
}

BigReal cut(const BigReal &x, unsigned int digits, DigitPool *digitPool) { // x truncated to the specified number of significant decimal digits
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  if(x.isZero() || (digits == 0)) {
    return pool->get0();
  }
  BigReal result(pool);
  unsigned int k = BigReal::getDecimalDigitCount(x.m_first->n);
  if(digits < k) {
    result.appendDigit(truncInt(x.m_first->n, digits - k));
    result.m_expo = result.m_low = x.m_expo;
  } else {
    result.m_expo = result.m_low = x.m_expo;
    result.appendDigit(x.m_first->n);
    digits -= k;
    const Digit *p;
    for(p = x.m_first->next; p && (digits >= LOG10_BIGREALBASE); p = p->next, digits -= LOG10_BIGREALBASE) {
      result.appendDigit(p->n);
      result.m_low--;
    }
    if(p && digits) { // if(p) then digits < LOG10_BIGREALBASE
      result.appendDigit(truncInt(p->n, digits-LOG10_BIGREALBASE));
      result.m_low--;
    }
  }
  if(x.isNegative()) {
    result.m_negative = true;
  }
  result.trimZeroes();
  SETBIGREALDEBUGSTRING(result);
  return result;
}

BigReal &BigReal::copyrTrunc(const BigReal &src, unsigned int digits) { // decimal digits
  if(src.isZero()) {
    setToZero();
    return *this;
  }
  const Digit   *sp = src.m_first;
  const unsigned k  = BigReal::getDecimalDigitCount(sp->n);
  if(digits <= k) {
    const unsigned int d = truncInt(sp->n, digits - k);
    if(m_first) {
      m_first->n = d;
      if(m_first->next) {
        deleteDigits(m_first->next, m_last);
        m_first->next = NULL;
        m_last = m_first;
      }
    } else {
      appendDigit(d);
    }
    m_low = m_expo = src.m_expo;
    m_negative = src.m_negative;
  } else {
    const int srcDecimals = (src.getLength() - 1) * LOG10_BIGREALBASE + k;
    if((int)digits >= srcDecimals) {
      return  *this = src;
    }
    Digit *dp;
    const int newLength   = (digits-k+LOG10_BIGREALBASE-1) / LOG10_BIGREALBASE + 1;
    if(isZero()) {
      appendDigit(sp->n);
      digits -= k;
      for(sp = sp->next; digits >= LOG10_BIGREALBASE; sp = sp->next, digits -= LOG10_BIGREALBASE) {
        appendDigit(sp->n);
      }
      dp = NULL;
    } else {
      dp    = m_first;
      dp->n = sp->n;
      digits -= k;
      for(dp = dp->next, sp = sp->next; dp && (digits >= LOG10_BIGREALBASE); sp = sp->next, dp = dp->next, digits -= LOG10_BIGREALBASE) {
        dp->n = sp->n;
      }
      for(;digits >= LOG10_BIGREALBASE; sp = sp->next, digits -= LOG10_BIGREALBASE) {
        appendDigit(sp->n);
      }
    }
    if(digits) {
      const unsigned int last = truncInt(sp->n, digits - LOG10_BIGREALBASE);
      if(dp) {
        dp->n = last;
        if(dp->next) {
          deleteDigits(dp->next, m_last);
          (m_last = dp)->next = NULL;
        }
      } else {
        appendDigit(last);
      }
    } else if(dp) {
      Digit *saveLast = m_last;
      (m_last = dp->prev)->next = NULL;
      deleteDigits(dp, saveLast);
    }
    m_low = (m_expo = src.m_expo) - newLength + 1;;
    if(m_last->n == 0) trimTail();
  }
  SETBIGREALDEBUGSTRING(*this);
  return *this;
}


// assume *this != 0 and digits > 0
BigReal &BigReal::rTrunc(unsigned int digits) {
  Digit             *first = m_first;
  const unsigned int k     = BigReal::getDecimalDigitCount(first->n);

  if(digits <= k) {
    first->n = truncInt(first->n, digits - k);
    if(first->next) {
      Digit *saveLast = m_last;
      m_last      = first;
      m_digitPool.deleteDigits(m_last->next, saveLast);
      first->next = NULL;
      m_low       = m_expo;
    }
  } else { // digits >= k
    int decLength = (getLength() - 1) * LOG10_BIGREALBASE + k;
    if((int)digits >= decLength) return *this; // nothing to cut

    const int newLength = (digits-k+LOG10_BIGREALBASE-1) / LOG10_BIGREALBASE + 1;
    Digit *p;

    if((int)digits <= decLength / 2) { // count from beginning
      for(p = first->next, digits -= k; digits > LOG10_BIGREALBASE; p = p->next, digits -= LOG10_BIGREALBASE);
    } else {                      // count from end
      for(p = m_last, decLength -= LOG10_BIGREALBASE; decLength > (int)digits; p = p->prev, decLength -= LOG10_BIGREALBASE);
      digits = digits - decLength;
    }

    if(p) {
      p->n = truncInt(p->n, digits-LOG10_BIGREALBASE);
      if(p->next) {
        Digit *saveLast = m_last;
        m_last = p;
        m_digitPool.deleteDigits(p->next, saveLast);
        p->next = NULL;
        m_low = m_expo - newLength + 1;
      }
      if(m_last->n == 0) trimTail();
    }
  }
  SETBIGREALDEBUGSTRING(*this);
  return *this;
}

// Assume *this != 0 and digits > 0
BigReal &BigReal::rRound(unsigned int digits) {
  Digit             *first = m_first;
  const unsigned int k     = BigReal::getDecimalDigitCount(first->n);
  if(digits < k) {
    if((first->n = roundInt(first->n, digits - k)) == BIGREALBASE) {
      first->n = 1;
      m_expo++;
      m_low++;
    }
    if(first->next) {
      Digit *saveLast = m_last;
      m_last      = first;
      m_digitPool.deleteDigits(m_last->next, saveLast);
      first->next = NULL;
      m_low       = m_expo;
    }
  } else { // digits >= k
    int decLength = (getLength() - 1) * LOG10_BIGREALBASE + k;
    if((int)digits >= decLength) return *this; // nothing to cut

    int       newLength    = (digits-k+LOG10_BIGREALBASE-1) / LOG10_BIGREALBASE + 1;
    const int wantedDigits = digits;
    Digit    *p;
    if((int)digits <= decLength / 2) { // count from beginning
      for(p = first->next, digits -= k; digits >= LOG10_BIGREALBASE; p = p->next, digits -= LOG10_BIGREALBASE);
    } else {                      // count from end
      for(p = m_last, decLength -= LOG10_BIGREALBASE; decLength > (int)digits; p = p->prev, decLength -= LOG10_BIGREALBASE);
      digits = digits - decLength;
    }

    if((p->n = roundInt(p->n, digits-LOG10_BIGREALBASE)) == BIGREALBASE) {
      for(p = p->prev; p; p = p->prev, newLength--) {
        if(++(p->n) < BIGREALBASE) { // add carry
          break;
        }
      }
      if(p) {
        Digit *saveLast = m_last;
        m_last = p;
        m_digitPool.deleteDigits(p->next, saveLast);
        p->next = NULL;
        m_low = m_expo - newLength + ((digits && ((wantedDigits-k)%LOG10_BIGREALBASE == digits)) ? 2 : 1); // !!
//        trimZeroes(); Not needed. p->n will always be at least 1
      } else { // carry propagated all the way up in front of m_first. set first to 1 and increment m_expo
        first->n = 1;
        m_digitPool.deleteDigits(first->next, m_last);
        first->next = NULL;
        m_last      = first;
        m_expo++;
        m_low = m_expo;
      }
    } else if(p->n == 0) {
      Digit *saveLast = m_last;
      m_last = p->prev;
      m_digitPool.deleteDigits(p, saveLast);
      m_last->next = NULL;
      m_low = m_expo - newLength + ((digits && ((wantedDigits-k)%LOG10_BIGREALBASE == digits)) ? 2 : 1); // !!
      if(m_last->n == 0) trimTail();
    } else if(p->next) {
      Digit *saveLast = m_last;
      m_last = p;
      m_digitPool.deleteDigits(p->next, saveLast);
      p->next = NULL;
      m_low = m_expo - newLength + 1;
//      trimZeroes(); Not needed. p->n != 0
    }
  }
  SETBIGREALDEBUGSTRING(*this);
  return *this;
}
