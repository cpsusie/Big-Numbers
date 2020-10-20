#include "pch.h"

BigReal fabs(const BigReal &x, DigitPool *digitPool) { // Absolute value of x (=|x|)
  _SELECTDIGITPOOL(x);
  if(!x.isNegative()) { // works for Nan too
    return BigReal(x,pool);
  }
  return BigReal(x, pool).clrFlags(BR_NEG);
}

int BigReal::logBASE(double x) { // static
  return (int)(log10(x) / BIGREAL_LOG10BASE);
}

BRExpoType getExpo2(const BigReal &x) {
  static const double log2_10 = 3.321928094887362; // = ln(10)/ln(2).
                                                   // so expo10(x) * log2_10 is approximately ln(x)/ln(10) * ln(10)/ln(2)
                                                   // = ln(x)/ln(2) = ln2(x) approximately  expo2(x)
  assert(x._isfinite());
  if(!x._isnormal()) {
    return 0;
  }
  const BRExpoType expo10 = BigReal::getExpo10(x);
  return (expo10 > -4890) ? ((BRExpoType)(log2_10 * expo10)) : ((BRExpoType)(log2(x.getFirst32(9))) + (BRExpoType)(log2_10 * (expo10-9)));
}


// Return number of decimal digits. 0 has length 1. undefined (nan,+inf,-inf) has length 0
size_t BigReal::getDecimalDigits() const {
  if(!_isnormal()) {
    return isZero() ? 1 : 0;
  }
  return (m_expo - m_low) * BIGREAL_LOG10BASE + getDecimalDigitCount(m_first->n) - getTrailingZeroCount(m_last->n);
}

BigReal e(const BigReal &x, BRExpoType n, DigitPool *pool) { // x * pow(10,n)
  return BigReal(x,pool).multPow10(n, true);
}

bool isEven(const BigReal &x) {
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

bool isOdd(const BigReal &x) {
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

BigInt floor(const BigReal &x, DigitPool *digitPool) { // biggest integer <= x
  return BigInt(x, digitPool);
}

BigInt ceil(const BigReal &x, DigitPool *digitPool) { // smallest integer >= x
  _SELECTDIGITPOOL(x);
  if(!isnormal(x)) return BigInt(x,pool);
  if(x.m_expo < 0) { // |x| < 1
    return x.isNegative() ? pool->_0() : pool->_1();
  } else if(isInteger(x)) { // x is a BigInt
    return BigInt(x,pool);
  } else {  // Copy the integerpart of x.
    BigInt result(pool);
    result.copyDigits(x, (result.m_expo = x.m_expo)+1);
    result.m_low      = 0;
    if(!result.copySign(x).trimZeroes().isNegative()) {
      result.clrInitDone();
      ++result;
      result.setInitDone();
    }
    return result;
  }
}

BigReal fraction(const BigReal &x, DigitPool *digitPool) { // sign(x) * (|x| - floor(|x|))
  _SELECTDIGITPOOL(x);
  if(!isfinite(x)) return BigReal(x, pool);
  BigReal result(pool);
  result.clrInitDone();
  x.fractionate(nullptr, &result);
  return result.setInitDone();
}

BigReal trunc(const BigReal &x, intptr_t prec, DigitPool *digitPool) {  // sign(x) * ent(abs(x)*10^prec)*10^-prec
  _SELECTDIGITPOOL(x);
  if(!isnormal(x)) return BigReal(x,pool);
  BigReal result(pool);
  result.clrInitDone();
  if(prec == 0) {
    result = (x.isNegative()) ? -floor(-x,pool) : floor(x,pool);
  } else {
    BigReal tmp(x,pool);
    result = (x.isNegative())
           ? -floor(-tmp.multPow10(prec,true)).multPow10(-prec,true)
           :  floor( tmp.multPow10(prec,true)).multPow10(-prec,true);
  }
  return result.setInitDone();
}

BigReal round(const BigReal &x, intptr_t prec, DigitPool *digitPool) { // sign(x) * ent(abs(x)*10^prec+0.5)*10^-prec
  _SELECTDIGITPOOL(x);
  if(!isnormal(x)) return BigReal(x,pool);
  BigReal result(pool);
  result.clrInitDone();
  if(prec == 0) {
    result = (x.isNegative())
           ? -floor(pool->_05() - x)
           :  floor(pool->_05() + x);
  } else {
    BigReal tmp(x, pool);
    result = (x.isNegative())
           ? -floor(pool->_05() - tmp.multPow10(prec,true)).multPow10(-prec,true)
           :  floor(pool->_05() + tmp.multPow10(prec,true)).multPow10(-prec,true);
  }
  return result.setInitDone();
}

void BigReal::fractionate(BigInt *integerPart, BigReal *fractionPart) const {
  DEFINEMETHODNAME;
  if(integerPart == fractionPart) {
    throwBigRealInvalidArgumentException(method, _T("integerPart is the same variable as fractionPart"));
  }
  if(integerPart ) CHECKISMUTABLE(*integerPart );
  if(fractionPart) CHECKISMUTABLE(*fractionPart);
  if(isZero()) {
    if(integerPart ) integerPart->setToZero();
    if(fractionPart) fractionPart->setToZero();
    return;
  }
  if(!_isnormal()) {
    if(integerPart ) integerPart->setToNan();
    if(fractionPart) fractionPart->setToNan();
    return;
  }

  if(isInteger(*this)) {
    if(integerPart ) *integerPart = (BigInt&)*this;
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
    intPart.copySign(*this);
    intPart.m_expo     = m_expo;
    intPart.m_low      = 0;

    Digit *dd = intPart.m_first;
    BRExpoType expo = m_expo;
    for(; dd && (expo-- >= 0); sd = sd->next, dd = dd->next) {
      dd->n = sd->n;
    }
    if(dd) {                             // remove excess digits from integerPart
      Digit *saveLast = intPart.m_last;
      intPart.m_last = dd->prev;
      intPart.m_last->next = nullptr;
      intPart.deleteDigits(dd, saveLast);
    } else {
      for(;expo-- >= 0; sd = sd->next) { // still some digits to append to integerpart
        intPart.appendDigit(sd->n);
      }
    }
    intPart.trimZeroes();
  } else { // Find first fractionPart, either from beginning or froam the end, depending on which is shortest
    if(m_expo <= -m_low) {
      for(BRExpoType expo = m_expo; expo-- >= 0; sd = sd->next);
    } else {
      sd = m_last;
      for(BRExpoType expo = m_low; ++expo < 0; sd = sd->prev);
    }
  }
  // Assume sd points to the first fraction digit
  if(fractionPart) {
    fractionPart->copySign(*this);
    fractionPart->m_expo     = -1;
    fractionPart->m_low      = m_low;
    Digit *dd;
    for(dd = fractionPart->m_first; dd && sd; dd = dd->next, sd = sd->next) {
      dd->n = sd->n;
    }
    if(dd) { // remove excess digits from fractionPart
      Digit *saveLast = fractionPart->m_last;
      fractionPart->m_last = dd->prev;
      fractionPart->m_last->next = nullptr;
      fractionPart->deleteDigits(dd, saveLast);
    } else {
      for(; sd; sd = sd->next) {
        fractionPart->appendDigit(sd->n);
      }
    }
    fractionPart->trimZeroes();
  }
}

static inline BRDigitType truncInt(BRDigitType n, int prec) { // assume prec <= 0
  if(prec == 0) {
    return n;
  } else {
    const BRDigitType p10 = BigReal::pow10(-prec);
    return (n / p10) * p10;
  }
}

static BRDigitType roundInt(BRDigitType n, int prec) { // assume prec <= 0
  if(prec == 0) {
    return n;
  } else {
    const BRDigitType p10 = BigReal::pow10(-prec-1);
    BRDigitType       v   = n / p10;
    const int         r   = v % 10;
    if(r >= 5) v += 10 - r; else v -= r;
    return v * p10;
  }
}

BigReal cut(const BigReal &x, size_t digits, DigitPool *digitPool) { // x truncated to the specified number of significant decimal digits
  _SELECTDIGITPOOL(x);
  if(!isfinite(x)) return BigReal(x,pool);
  if(x.isZero() || (digits == 0)) {
    return pool->_0();
  }
  BigReal result(pool);
  result.clrInitDone();
  int k = BigReal::getDecimalDigitCount(x.m_first->n);
  if(digits < (UINT)k) {
    result.appendDigit(truncInt(x.m_first->n, (int)digits - k));
    result.m_expo = result.m_low = x.m_expo;
  } else {
    result.m_expo = result.m_low = x.m_expo;
    result.appendDigit(x.m_first->n);
    digits -= k;
    const Digit *p;
    for(p = x.m_first->next; p && (digits >= BIGREAL_LOG10BASE); p = p->next, digits -= BIGREAL_LOG10BASE) {
      result.appendDigit(p->n);
      result.m_low--;
    }
    // invariant: if(p) then digits < BIGREAL_LOG10BASE
    if(p && digits) { // if(p) then digits < BIGREAL_LOG10BASE
      result.appendDigit(truncInt(p->n, (int)digits-BIGREAL_LOG10BASE));
      result.m_low--;
    }
  }
  return result.copySign(x).trimZeroes().setInitDone();
}

BigReal &BigReal::copyrTrunc(const BigReal &src, size_t digits) { // decimal digits
  if(!isnormal(src)) {
    *this = src;
    return *this;
  }
  CHECKISMUTABLE(*this);
  const Digit *sp = src.m_first;
  const int    k  = getDecimalDigitCount(sp->n);
  if(digits <= (UINT)k) {
    const BRDigitType d = truncInt(sp->n, (int)digits - k);
    if(m_first) {
      m_first->n = d;
      if(m_first->next) {
        deleteDigits(m_first->next, m_last);
        m_first->next = nullptr;
        m_last = m_first;
      }
    } else {
      appendDigit(d);
    }
    m_low = m_expo = src.m_expo;
    copySign(src);
  } else {
    const intptr_t srcDecimals = (src.getLength() - 1) * BIGREAL_LOG10BASE + k;
    if((intptr_t)digits >= srcDecimals) {
      return  *this = src;
    }
    Digit *dp;
    const intptr_t newLength   = (digits-k+BIGREAL_LOG10BASE-1) / BIGREAL_LOG10BASE + 1;
    if(isZero()) {
      appendDigit(sp->n);
      digits -= k;
      for(sp = sp->next; digits >= BIGREAL_LOG10BASE; sp = sp->next, digits -= BIGREAL_LOG10BASE) {
        appendDigit(sp->n);
      }
      dp = nullptr;
    } else {
      dp    = m_first;
      dp->n = sp->n;
      digits -= k;
      for(dp = dp->next, sp = sp->next; dp && (digits >= BIGREAL_LOG10BASE); sp = sp->next, dp = dp->next, digits -= BIGREAL_LOG10BASE) {
        dp->n = sp->n;
      }
      for(;digits >= BIGREAL_LOG10BASE; sp = sp->next, digits -= BIGREAL_LOG10BASE) {
        appendDigit(sp->n);
      }
    }
    if(digits) { // invariant: digits < BIGREAL_LOG10BASE
      const BRDigitType last = truncInt(sp->n, (int)digits - BIGREAL_LOG10BASE);
      if(dp) {
        dp->n = last;
        if(dp->next) {
          deleteDigits(dp->next, m_last);
          (m_last = dp)->next = nullptr;
        }
      } else {
        appendDigit(last);
      }
    } else if(dp) {
      Digit *saveLast = m_last;
      (m_last = dp->prev)->next = nullptr;
      deleteDigits(dp, saveLast);
    }
    m_low = (m_expo = src.m_expo) - newLength + 1;
    if(m_last->n == 0) trimTail();
  }
  return *this;
}

// assume _isnormal() and digits > 0
BigReal &BigReal::rTrunc(size_t digits) {
  assert(_isnormal());
  CHECKISMUTABLE(*this);
  Digit    *first = m_first;
  const int k     = BigReal::getDecimalDigitCount(first->n);

  if(digits <= (UINT)k) {
    first->n = truncInt(first->n, (int)digits - k);
    if(first->next) {
      Digit *saveLast = m_last;
      m_last      = first;
      deleteDigits(m_last->next, saveLast);
      first->next = nullptr;
      m_low       = m_expo;
    }
  } else { // digits >= k
    intptr_t decLength = (getLength() - 1) * BIGREAL_LOG10BASE + k;
    if((intptr_t)digits >= decLength) return *this; // nothing to cut

    const intptr_t newLength = (digits-k+BIGREAL_LOG10BASE-1) / BIGREAL_LOG10BASE + 1;
    Digit *p;

    if((intptr_t)digits <= decLength / 2) { // count from beginning
      for(p = first->next, digits -= k; digits > BIGREAL_LOG10BASE; p = p->next, digits -= BIGREAL_LOG10BASE);
    } else {                      // count from end
      for(p = m_last, decLength -= BIGREAL_LOG10BASE; decLength > (intptr_t)digits; p = p->prev, decLength -= BIGREAL_LOG10BASE);
      digits = digits - decLength; // invariant: decLength <= digits && digits - decLength <= BIGREAL_LOG10BASE
    }

    if(p) { // invariant: digits <= BIGREAL_LOG10BASE
      p->n = truncInt(p->n, (int)digits-BIGREAL_LOG10BASE);
      if(p->next) {
        Digit *saveLast = m_last;
        m_last = p;
        deleteDigits(p->next, saveLast);
        p->next = nullptr;
        m_low = m_expo - newLength + 1;
      }
      if(m_last->n == 0) trimTail();
    }
  }
  return *this;
}

// Assume *this != 0 and digits > 0
BigReal &BigReal::rRound(size_t digits) {
  assert(_isnormal());
  CHECKISMUTABLE(*this);
  Digit    *first = m_first;
  const int k     = getDecimalDigitCount(first->n);
  if(digits < (UINT)k) {
    if((first->n = roundInt(first->n, (int)digits - k)) == BIGREALBASE) {
      first->n = 1;
      m_expo++;
      m_low++;
    }
    if(first->next) {
      Digit *saveLast = m_last;
      m_last      = first;
      deleteDigits(m_last->next, saveLast);
      first->next = nullptr;
      m_low       = m_expo;
    }
  } else { // digits >= k
    intptr_t decLength = (getLength() - 1) * BIGREAL_LOG10BASE + k;
    if((intptr_t)digits >= decLength) return *this; // nothing to cut

    intptr_t       newLength    = (digits-k+BIGREAL_LOG10BASE-1) / BIGREAL_LOG10BASE + 1;
    const intptr_t wantedDigits = digits;
    Digit    *p;
    if((intptr_t)digits <= decLength / 2) { // count from beginning
      for(p = first->next, digits -= k; digits >= BIGREAL_LOG10BASE; p = p->next, digits -= BIGREAL_LOG10BASE);
    } else {                      // count from end
      for(p = m_last, decLength -= BIGREAL_LOG10BASE; decLength > (intptr_t)digits; p = p->prev, decLength -= BIGREAL_LOG10BASE);
      digits = digits - decLength;
    }

    if((p->n = roundInt(p->n, (int)digits-BIGREAL_LOG10BASE)) == BIGREALBASE) {
      for(p = p->prev; p; p = p->prev, newLength--) {
        if(++(p->n) < BIGREALBASE) { // add carry
          break;
        }
      }
      if(p) {
        Digit *saveLast = m_last;
        m_last = p;
        deleteDigits(p->next, saveLast);
        p->next = nullptr;
        m_low = m_expo - newLength + ((digits && ((wantedDigits-k)%BIGREAL_LOG10BASE == digits)) ? 2 : 1); // !!
//        trimZeroes(); Not needed. p->n will always be at least 1
      } else { // carry propagated all the way up in front of m_first. set first to 1 and increment m_expo
        first->n = 1;
        deleteDigits(first->next, m_last);
        first->next = nullptr;
        m_last      = first;
        m_expo++;
        m_low = m_expo;
      }
    } else if(p->n == 0) {
      Digit *saveLast = m_last;
      m_last = p->prev;
      deleteDigits(p, saveLast);
      m_last->next = nullptr;
      m_low = m_expo - newLength + ((digits && ((wantedDigits-k)%BIGREAL_LOG10BASE == digits)) ? 2 : 1); // !!
      if(m_last->n == 0) trimTail();
    } else if(p->next) {
      Digit *saveLast = m_last;
      m_last = p;
      deleteDigits(p->next, saveLast);
      p->next = nullptr;
      m_low = m_expo - newLength + 1;
//      trimZeroes(); Not needed. p->n != 0
    }
  }
  return *this;
}
