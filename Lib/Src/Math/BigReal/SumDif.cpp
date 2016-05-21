#include "pch.h"

// Assume *this != 0.
// Return Digit corresponding to BIGREALBASE^exponent.
// Return NULL if exponent is outside interval [m_low;m_expo]
Digit *BigReal::findDigit(const int exponent) const { 
  int fwd = m_expo - exponent;
  int bwd = exponent - m_low;
  
  if(bwd < 0 || fwd < 0) {
    return NULL;
  } else if(bwd < fwd) { // count backward
    for(Digit *p = m_last;  bwd--; p = p->prev);
    return p;
  } else {               // count forward
    for(Digit *p = m_first; fwd--; p = p->next);
    return p;
  }
}

// Assume *this != 0.
// Return last digit with precision f. Used in add
// Return NULL if abs(this) < precisionen (=f)
// low is set to the max(fexpo,m_low), where fexpo = f.expo - [f.first == 1]
// Differs from findDigitSubtract in the special case where f = BIGREALBASE^n for some n
Digit *BigReal::findDigitAdd(const BigReal &f, int &low) const { 
  if(!f.isPositive()) {
    low = m_low;
    return m_last;
  } 
  int fexpo = f.m_expo;
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

    int bwd = fexpo  - m_low;
    int fwd = m_expo - fexpo;
    if(bwd < fwd) { // count backward
      for(Digit *p = m_last;  bwd--; p = p->prev);
      return p;
    } else {        // count forward
      for(Digit *p = m_first; fwd--; p = p->next);
      return p;
    }
  }
}

// Assume *this != 0.
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
    int bwd = f.m_expo - m_low;
    int fwd = m_expo   - f.m_expo;
    if(bwd < fwd) { // count backward
      for(Digit *p = m_last;  bwd--; p = p->prev);
      return p;
    } else {        // count forward
      for(Digit *p = m_first; fwd--; p = p->next);
      return p;
    }
  }
}

// Assume &x != this && &y != this && *this == 0, x != 0 && y != 0
// Return *this = |x| + |y| with maximal error = f
BigReal &BigReal::addAbs(const BigReal &x, const BigReal &y, const BigReal &f) {
  int xLow, yLow;
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
    if(xLow < yLow) {
      for(int d = yLow - xLow; xp && d--; xp = xp->prev) {
        insertDigit(xp->n);
      }          
      if(d > 0) {
        insertZeroDigits(d);
      }
    } else if(yLow < xLow) {
      for(int d = xLow - yLow; yp && d--; yp = yp->prev) {
        insertDigit(yp->n);
      }
      if(d > 0) {
        insertZeroDigits(d);
      }
    }
    for(unsigned long carry = 0; xp && yp; xp = xp->prev, yp = yp->prev) {
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

// Assume &x != this && *this != 0 && x != 0
// Adds |x| to |this|.
// Return *this
BigReal &BigReal::addAbs(const BigReal &x) {
  const Digit *xp;
  Digit *p;
  if(x.m_low < m_low) {
    p  = m_last;
    xp = x.m_last;
    const int t = min(m_low-1, x.m_expo);
    for(int d = x.m_low; d <= t; d++, xp = xp->prev) {
      insertAfter(p, xp->n);
    }
    if(d < m_low) {
      insertZeroDigitsAfter(p, m_low-d);
    }
    m_low = x.m_low;
  } else if(x.m_low > m_low) { 
    if((p = findDigit(x. m_low)) == NULL) {
      const int t = x.m_low - 1 - m_expo;
      if(t > 0) {
        insertZeroDigits(t);
      }
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

  unsigned long carry = 0;
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

// Assume &x != this && *this != 0 && x != 0 && |this| > |x|
// Subtract |x| from |this| with maximal error = f
BigReal &BigReal::subAbs(const BigReal &x, const BigReal &f) { 
  const Digit *xp = x.findDigitSubtract(f);

  if(xp == NULL) {
    return *this; // nothing to subtract
  }

  Digit *p = findDigitSubtract(f); 
  const int last = f.isPositive() ? max(f.m_expo, x.m_low) : x.m_low;
  if(last < m_low) {
    const int k = m_low;
    p->n--;              // borrow
    insertAfter(p, BIGREALBASE - xp->n);
    xp = xp->prev;
    m_low--;
    for(int d = k - (last+1); xp && (d-- > 0); xp = xp->prev) {
      insertAfter(p, BIGREALBASE - 1 - xp->n);
      m_low--;
    }
    if(d > 0) {
      insertBorrowDigitsAfter(p, d);
      m_low -= d;
    }
  } else if(last > m_low) {
    for(int d = last - m_low; p && d--; p = p->prev);
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
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  if(x.isZero()) {
    return BigReal(y, pool);
  } else if(y.isZero()) {
    return digitPool ? BigReal(x, digitPool) : x;
  }

  // x != 0 && y != 0
  if(x.m_negative == y.m_negative) { // sign(x) == sign(y). Calculate sign(x) * (|x| + |y|)
    BigReal result(pool);
    result.addAbs(x, y, f);
    result.m_negative = x.m_negative;
    SETBIGREALDEBUGSTRING(result);
    return result;
  } else if(f.isZero()) {            // sign(x) != sign(y)
    const int c = compareAbs(x, y);
    if(c > 0) {                      // |x| > |y|. Calculate sign(x) * (|x| - |y|)
      BigReal result(x, pool);
      result.subAbs(y, f);
      SETBIGREALDEBUGSTRING(result);
      return result;
    } else if(c < 0) {               // |x| < |y|. Calculate sign(y) * (|y| - |x|)
      BigReal result(y, pool);
      result.subAbs(x, f);
      SETBIGREALDEBUGSTRING(result);
      return result;
    } else {                         // x == -y => x + y = 0
      return pool->get0();
    }
  } else {                           // f != 0
    BigReal tmpX(pool), tmpY(pool);
    copy(tmpX, x, f);
    copy(tmpY, y, f);
    const int c = compareAbs(tmpX, tmpY);
    if(c > 0) {                      // abs(tmpX) > abs(tmpY)
      tmpX.subAbs(tmpY, f);
      SETBIGREALDEBUGSTRING(tmpX);
      return tmpX;
    } else if(c < 0) {               // abs(tmpX) < abs(tmpY)
      tmpY.subAbs(tmpX, f);
      SETBIGREALDEBUGSTRING(tmpY);
      return tmpY;
    } else {                         // tmpX == -tmpY => tmpX + tmpY == 0
      return pool->get0();
    }
  }
}

BigReal dif(const BigReal &x, const BigReal &y, const BigReal &f,  DigitPool *digitPool) {
  DigitPool *pool = digitPool ? digitPool : x.getDigitPool();
  if(y.isZero()) {
    return digitPool ? BigReal(x, digitPool) : x;
  } else if(x.isZero()) {
    return -BigReal(y, pool);
  }

  // x != 0 && y != 0
  if(x.m_negative != y.m_negative) { // sign(x) != sign(y). Calculate sign(x) * (|x| + |y|)
    BigReal result(pool);
    result.addAbs(x, y, f);
    result.m_negative = x.m_negative;
    SETBIGREALDEBUGSTRING(result)
    return result;
  } else if(f.isZero()) {            // sign(x) == sign(y)
    const int c = compareAbs(x, y);
    if(c > 0) {                      // |x| > |y|. Calculate sign(x) * (|x| - |y|)
      BigReal result(x, pool);
      result.subAbs(y, f);
      SETBIGREALDEBUGSTRING(result)
      return result;
    } else if(c < 0) {               // |x| < |y|. Calculate -sign(y) * (|y| - |x|)
      BigReal result(y, pool);
      result.subAbs(x, f);
      result.changeSign();
      SETBIGREALDEBUGSTRING(result)
      return result;
    } else {                         // x == y => x - y = 0;
      return pool->get0();
    }
  } else {                           // f != 0
    BigReal tmpX(pool), tmpY(pool);
    copy(tmpX, x, f);
    copy(tmpY, y, f);
    const int c = compareAbs(tmpX, tmpY);
    if(c > 0) {                      // abs(tmpX) > abs(tmpY). Calculate sign(x) * (abs(tmpX) - abs(tmpY))
      tmpX.subAbs(tmpY, f);
      SETBIGREALDEBUGSTRING(tmpX)
      return tmpX;
    } else if(c < 0) {               // abs(tmpX) < abs(tmpY). Calcluate -sign(tmpY) * (abs(tmpY) - abs(tmpX))
      tmpY.subAbs(tmpX, f);
      tmpY.changeSign();
      SETBIGREALDEBUGSTRING(tmpY)
      return tmpY;
    } else {                         // tmpX == tmpY => tmpX - tmpY == 0;
      return pool->get0();
    }
  }
}

BigReal operator+(const BigReal &x, const BigReal &y) {
  return sum(x, y, x.getDigitPool()->get0());
}

BigReal operator-(const BigReal &x) {
  if(x.isZero()) {
    return x;
  }
  BigReal result(x);
  result.changeSign();
  SETBIGREALDEBUGSTRING(result)
  return result;
}

BigReal operator-(const BigReal &x, const BigReal &y) {
  return dif(x, y, x.getDigitPool()->get0());
}

BigReal &BigReal::operator+=(const BigReal &x) {
  if(&x == this) {
    if(isZero()) {
      return *this;
    } else {
      const BigReal tmp(x, getDigitPool());
      return *this += tmp;
    }
  }
  if(isZero()) {
    return *this = x;
  } else if(x.isZero()) {
    return *this;
  }

  // *this != 0 && x != 0
  if(x.m_negative == m_negative) {   // sign(this) == sign(x). Calculate (sign(this) * (|this| + |x|)
    addAbs(x); 
  } else {                           // sign(this) != sign(x)
    const int c = compareAbs(*this, x);
    if(c > 0) {                      // |this| > |x|. Calculate sign(this) * (|this| - |x|)
      subAbs(x, BIGREAL_0);
    } else if(c < 0) {               // |this| < |x|. Calculate sign(x) * (|x| - |this|)
      BigReal tmp(x,getDigitPool());
      *this = tmp.subAbs(*this, BIGREAL_0);
    } else {                         // |this| == |x| => *this = 0
      *this = BIGREAL_0;
    }
  }                        
  SETBIGREALDEBUGSTRING(*this)
  return *this;
}

BigReal &BigReal::operator-=(const BigReal &x) {
  if(&x == this) {
    return *this = BIGREAL_0;
  } else if(isZero()) {
    *this = x;
    changeSign();
    SETBIGREALDEBUGSTRING(*this)
    return *this;
  } else if(x.isZero()) {
    return *this;
  }

  // *this != 0 && x != 0
  if(x.m_negative != m_negative) {   // sign(this) != sign(x)
    addAbs(x);
  } else {                           // sign(this) == sign(x)
    const int c = compareAbs(*this, x);
    if(c > 0) {                      // |this| > |x|. Calculcate sign(this) * (|this| - |x|)
      subAbs(x, BIGREAL_0);
    } else if(c < 0) {               // |this| < |x|. Calculate -sign(x) * (|x| - |this|)
      BigReal tmp(x);
      *this = tmp.subAbs(*this, BIGREAL_0);
      changeSign();
    } else {                         // |this| == |x| => *this = 0
      *this = BIGREAL_0;
    }
  }
  SETBIGREALDEBUGSTRING(*this)
  return *this;
}

BigReal &BigReal::operator++() {         // prefix-form
  *this += BIGREAL_1;
  return *this;
}

BigReal &BigReal::operator--() {         // prefix-form
  *this -= BIGREAL_1;
  return *this;
}

BigReal BigReal::operator++(int dummy) { // postfix-form
  BigReal result(*this);
  *this += BIGREAL_1;
  return result;
}

BigReal BigReal::operator--(int dummy) { // postfix-form
  BigReal result(*this);
  *this -= BIGREAL_1;
  return result;
}
