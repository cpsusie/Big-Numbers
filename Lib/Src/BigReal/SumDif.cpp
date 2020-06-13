#include "pch.h"

// Assume this->_isnormal()
// Return Digit corresponding to BIGREALBASE^exponent.
// Return NULL if exponent is outside interval [m_low;m_expo]
Digit *BigReal::findDigit(const BRExpoType exponent) const {
  BRExpoType fwd = m_expo - exponent;
  BRExpoType bwd = exponent - m_low;
  Digit *p;
  if((bwd < 0) || (fwd < 0)) {
    return NULL;
  } else if(bwd < fwd) { // count backward
    for(p = m_last;  bwd--; p = p->prev);
  } else {               // count forward
    for(p = m_first; fwd--; p = p->next);
  }
  return p;
}

// Assume this->_isnormal()
// Return last digit with precision f. Used in add
// Return NULL if abs(this) < precisionen (=f)
// low is set to the max(fexpo,m_low), where fexpo = f.expo - [f.first == 1]
// Differs from findDigitSubtract in the special case where f = BIGREALBASE^n for some n
Digit *BigReal::findDigitAdd(const BigReal &f, BRExpoType &low) const {
  if(!f.isPositive()) {
    low = m_low;
    return m_last;
  }
  BRExpoType fexpo = f.m_expo;
  if(f.m_first->n == 1) {
    fexpo--;      // NB NB NB!
  }
  if(fexpo > m_expo) {
    low = f.m_expo;
    return NULL;
  } else if(fexpo <= m_low) {
    low = m_low;
    return m_last;
  } else {
    low = fexpo;

    BRExpoType bwd = fexpo  - m_low;
    BRExpoType fwd = m_expo - fexpo;
    Digit *p;
    if(bwd < fwd) { // count backward
      for(p = m_last;  bwd--; p = p->prev);
    } else {        // count forward
      for(p = m_first; fwd--; p = p->next);
    }
    return p;
  }
}

// Assume this->_isnormal()
// Return last digit with precision f. Used in subtract
// Return NULL if |this| < precisionen (=f)
Digit *BigReal::findDigitSubtract(const BigReal &f) const {
  if(!f.isPositive()) {
    return m_last;
  } else if(f.m_expo > m_expo) {
    return NULL;
  } else if(f.m_expo <= m_low) {
    return m_last;
  } else {
    BRExpoType bwd = f.m_expo - m_low;
    BRExpoType fwd = m_expo   - f.m_expo;
    Digit *p;
    if(bwd < fwd) { // count backward
      for(p = m_last;  bwd--; p = p->prev);
    } else {        // count forward
      for(p = m_first; fwd--; p = p->next);
    }
    return p;
  }
}

// Assume &x != this && &y != this && *this == 0, x._isnormal() && y._isnormal()
// Return *this = |x| + |y| with maximal error = f
BigReal &BigReal::addAbs(const BigReal &x, const BigReal &y, const BigReal &f) {
  assert((&x != this) && (&y != this) && isZero() && x._isnormal() && y._isnormal());

  BRExpoType xLow, yLow;
  const Digit *xp = x.findDigitAdd(f, xLow);
  const Digit *yp = y.findDigitAdd(f, yLow);

  if(xp == NULL && yp == NULL) {
    return *this;
  } else if(xp == NULL) {
    for(;yp; yp = yp->prev) {
      insertDigit(yp->n);
    }
    m_expo = y.m_expo;
    m_low  = yLow;
  } else if(yp == NULL) {
    for(;xp; xp = xp->prev) {
      insertDigit(xp->n);
    }
    m_expo = x.m_expo;
    m_low  = xLow;
  } else { // xp != NULL && yp != NULL
    BRExpoType d;
    if(xLow < yLow) {
      for(d = yLow - xLow; xp && d--; xp = xp->prev) {
        insertDigit(xp->n);
      }
      if(d > 0) insertZeroDigits(d);
    } else if(yLow < xLow) {
      for(d = xLow - yLow; yp && d--; yp = yp->prev) {
        insertDigit(yp->n);
      }
      if(d > 0) insertZeroDigits(d);
    }
    BRDigitType carry;
    for(carry = 0; xp && yp; xp = xp->prev, yp = yp->prev) {
      carry += xp->n + yp->n;
      insertDigit(carry % BIGREALBASE);
      carry /= BIGREALBASE;
    }
    if(xp) {
      do {
        carry += xp->n;
        xp = xp->prev;
        insertDigit(carry % BIGREALBASE);
        carry /= BIGREALBASE;
      } while(xp);
    } else if(yp) {
      do {
        carry += yp->n;
        yp = yp->prev;
        insertDigit(carry % BIGREALBASE);
        carry /= BIGREALBASE;
      } while(yp);
    }
    m_expo = max(x.m_expo,y.m_expo);
    if(carry) {
      insertDigit(carry);
      m_expo++;
    }
    m_low = min(xLow,yLow);
  }
  return trimZeroes();
}

// Assume &x != this && this->_isnormal() && x._isnormal()
// Adds |x| to |this|.
// Return *this
BigReal &BigReal::addAbs(const BigReal &x) {
  assert((&x != this) && _isnormal() && x._isnormal());
  const Digit *xp;
  Digit *p;
  if(x.m_low < m_low) {
    p  = m_last;
    xp = x.m_last;
    const BRExpoType t = min(m_low-1, x.m_expo);
    BRExpoType d;
    for(d = x.m_low; d <= t; d++, xp = xp->prev) {
      insertAfter(p, xp->n);
    }
    if(d < m_low) insertZeroDigitsAfter(p, m_low-d);
    m_low = x.m_low;
  } else if(x.m_low > m_low) {
    if((p = findDigit(x. m_low)) == NULL) {
      const BRExpoType t = x.m_low - 1 - m_expo;
      if(t > 0) insertZeroDigits(t);
      for(xp = x.m_last; xp; xp = xp->prev) {
        insertDigit(xp->n);
      }
      m_expo = x.m_expo;
      return trimZeroes();
    }
    xp = x.m_last;
  } else {
    p = m_last;
    xp = x.m_last;
  }

  BRDigitType carry = 0;
  while(xp) {
    if(p) {
      carry += p->n + xp->n;
      p->n = carry % BIGREALBASE;
      p = p->prev;
    } else {
      carry += xp->n;
      insertDigit(carry % BIGREALBASE);
      m_expo++;
    }
    carry /= BIGREALBASE;
    xp = xp->prev;
  }
  while(carry) {
    if(p) {
      carry += p->n;
      p->n = carry % BIGREALBASE;
      p = p->prev;
    } else {
      insertDigit(carry % BIGREALBASE);
      m_expo++;
    }
    carry /= BIGREALBASE;
  }
  return trimZeroes();
}

// Assume &x != this && this->_isnormal() && x._isnormal() && |x| < |*this|
// Subtract |x| from |this| with maximal error = f
BigReal &BigReal::subAbs(const BigReal &x, const BigReal &f) {
  assert((&x != this) && _isnormal() && x._isnormal() && (compareAbs(x, *this) < 0));
  const Digit *xp = x.findDigitSubtract(f);

  if(xp == NULL) {
    return *this; // nothing to subtract
  }

  Digit *p = findDigitSubtract(f);
  const BRExpoType last = f.isPositive() ? max(f.m_expo, x.m_low) : x.m_low;
  if(last < m_low) {
    const BRExpoType k = m_low;
    p->n--;              // borrow
    insertAfter(p, BIGREALBASE - xp->n);
    xp = xp->prev;
    m_low--;
    BRExpoType d;
    for(d = k - (last+1); xp && (d-- > 0); xp = xp->prev) {
      insertAfter(p, BIGREALBASE - 1 - xp->n);
      m_low--;
    }
    if(d > 0) {
      insertBorrowDigitsAfter(p, d);
      m_low -= d;
    }
  } else if(last > m_low) {
    for(BRExpoType d = last - m_low; p && d--; p = p->prev);
  }

  while(xp) {
    if(p->n >= xp->n) {
      p->n -= xp->n;
      p = p->prev, xp = xp->prev;
    } else { // Borrow. We know there is one digit further up the chain we can borrow from
      p->n +=  BIGREALBASE - xp->n;
      for(p = p->prev, xp = xp->prev; (p->n == 0) && xp; p = p->prev, xp = xp->prev) {
        p->n = BIGREALBASE - 1 - xp->n;
      }
      for(;p->n == 0; p = p->prev) {
        p->n = BIGREALBASE - 1;
      }
      p->n--;
    }
  }
  return trimZeroes();
}

BigReal sum(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!x._isfinite() || !y._isfinite() || !f._isfinite()) return pool->nan();
  if(x.isZero()) {
    return BigReal(y, pool);
  } else if(y.isZero()) {
    return BigReal(x, pool);
  }

  // x != 0 && y != 0
  if(hasSameSign(x,y)) {                                                                    // sign(x) == sign(y).
    return BigReal(pool).addAbs(x, y, f).copySign(x);                                       // return sign(x) * (|x| + |y|)
  } else if(f.isZero()) {                                                                   // sign(x) != sign(y)
    switch(BigReal::compareAbs(x, y)) {
    case  1: return BigReal(x, pool).subAbs(y, f);                                          // |x| >  |y|. return   x -= |y|
    case -1: return BigReal(y, pool).subAbs(x, f);                                          // |x| <  |y|. return   y -= |x|
    case  0: return pool->_0();                                                             // |x| == |y|. return   0
    default: NODEFAULT;
    }
  } else {                                                                                  // f != 0
    BigReal tmpX(pool), tmpY(pool);
    copy(tmpX.clrInitDone(), x, f).setInitDone();
    copy(tmpY.clrInitDone(), y, f).setInitDone();
    switch(BigReal::compareAbs(tmpX, tmpY)) {
    case  1: return tmpY._isnormal() ? tmpX.subAbs(tmpY, f) : tmpX;                        // |tmpX| >  |tmpY|. return   tmpX -= |tmpY|
    case -1: return tmpX._isnormal() ? tmpY.subAbs(tmpX, f) : tmpY;                        // |tmpX| <  |tmpY|. return   tmpY -= |tmpX|
    case  0: return pool->_0();                                                            // |tmpX| == |tmpY|. return   0
    default: NODEFAULT;
    }
  }
  return pool->_0(); // shoul not come here
}

BigReal dif(const BigReal &x, const BigReal &y, const BigReal &f, DigitPool *digitPool) {
  _SELECTDIGITPOOL(x);
  if(!x._isfinite() || !y._isfinite() || !f._isfinite()) return pool->nan();
  if(y.isZero()) {
    return BigReal(x, pool);
  } else if(x.isZero()) {
    return BigReal(y, pool).flipSign();
  }

  // x != 0 && y != 0 && f._isfinite()
  if(!hasSameSign(x,y)) {                                                                   // sign(x) != sign(y).
    return BigReal(pool).addAbs(x, y, f).copySign(x);                                       // return sign(x) * (|x| + |y|)
  } else if(f.isZero()) {                                                                   // sign(x) == sign(y)
    switch(BigReal::compareAbs(x, y)) {
    case  1: return BigReal(x, pool).subAbs(y, f);                                          // |x| >  |y|. return   x -= |y|
    case -1: return BigReal(y, pool).subAbs(x, f).flipSign();                               // |x| <  |y|. return -(y -= |x|)
    case  0: return pool->_0();                                                             // |x| == |y|. return   0
    default: NODEFAULT;
    }
  } else {                                                                                  // f != 0
    BigReal tmpX(pool), tmpY(pool);
    copy(tmpX.clrInitDone(), x, f).setInitDone();
    copy(tmpY.clrInitDone(), y, f).setInitDone();;
    switch(BigReal::compareAbs(tmpX, tmpY)) {
    case  1: return tmpY._isnormal() ? tmpX.subAbs(tmpY, f) : tmpX;                         // |tmpX| >  |tmpY|. return   tmpX -= |tmpY|
    case -1: return tmpX._isnormal() ? tmpY.subAbs(tmpX, f).flipSign() : tmpY.flipSign();   // |tmpX| <  |tmpY|. return -(tmpY -= |tmpX|)
    case  0: return pool->_0();                                                             // |tmpX| == |tmpY|. return   0
    default: NODEFAULT;
    }
  }
  return pool->_0(); // shoul not come here
}

BigReal operator+(const BigReal &x, const BigReal &y) {
  return sum(x, y, BigReal::_0);
}

BigReal operator-(const BigReal &x) {
  return x._hasSign() ? BigReal(x).flipSign() : x;
}

BigReal operator-(const BigReal &x, const BigReal &y) {
  return dif(x, y, BigReal::_0);
}

BigReal &BigReal::operator+=(const BigReal &x) {
  if(&x == this) {
    if(!_isnormal()) {
      return *this;
    } else {
      return multiply2();
    }
  }
  if(!_isfinite() || !x._isfinite()) return setToNan();
  if(isZero()) {
    return *this = x;
  } else if(x.isZero()) {
    return *this;
  }

  CHECKISMUTABLE(*this);
                                                                                   // this->_isnormal() && x._isnormal()
  if(hasSameSign(*this,x)) {                                                       // sign(this) == sign(x). *this += |x|
    return addAbs(x);
  } else {                                                                         // sign(this) != sign(x)
    switch(compareAbs(*this, x)) {
    case  1: return subAbs(x);                                                     // |this| >  |x|. *this -= |x|
    case -1: return *this = BigReal(x, getDigitPool()).subAbs(*this);              // |this| <  |x|. *this = x - |*this|
    case  0: return setToZero();                                                   // |this| == |x|. *this = 0
    default: NODEFAULT;
    }
  }
  return *this;
}

BigReal &BigReal::operator-=(const BigReal &x) {
  if(!_isfinite() || !x._isfinite()) return setToNan();
  if(&x == this) {
    return setToZero();
  } else if(isZero()) {
    *this = x;
    return changeSign();
  } else if(x.isZero()) {
    return *this;
  }

  CHECKISMUTABLE(*this);
                                                                                   // this->_isnormal() && x._isnormal()
  if(!hasSameSign(*this, x)) {                                                     // sign(this) != sign(x). *this += |x|
    return addAbs(x);
  } else {                                                                         // sign(this) == sign(x)
    switch(compareAbs(*this, x)) {
    case  1: return subAbs(x);                                                     // |this| >  |x|. *this -= |x|
    case -1: return *this = BigReal(x, getDigitPool()).subAbs(*this).flipSign();   // |this| <  |x|. *this = -(x - |*this|)
    case  0: return setToZero();                                                   // |this| == |x|. *this = 0
    default: NODEFAULT;
    }
  }
  return *this;
}

BigReal &BigReal::operator++() {         // prefix-form
  return *this += m_digitPool._1();
}

BigReal &BigReal::operator--() {         // prefix-form
  return *this -= m_digitPool._1();
}

BigReal BigReal::operator++(int dummy) { // postfix-form
  BigReal result(*this);
  *this += m_digitPool._1();
  return result;
}

BigReal BigReal::operator--(int dummy) { // postfix-form
  BigReal result(*this);
  *this -= m_digitPool._1();
  return result;
}
