#include "pch.h"
#include <float.h>
#include <ctype.h>

void BigReal::init(int n) {
  initToZero();
  if(n) {
    BRDigitType un;
    if(n < 0) {
      flipSign();
      un = -n;
    } else {
      un = n;
    }
    m_expo = -1;
    do {
      insertDigit(un % BIGREALBASE);
      un /= BIGREALBASE;
      m_expo++;
    } while(un);
    trimZeroes();
  }
}

void BigReal::init(UINT n) {
  initToZero();
  if(n) {
    m_expo = -1;
    BRDigitType d = n;
    do {
      insertDigit(d % BIGREALBASE);
      d /= BIGREALBASE;
      m_expo++;
    } while(d);
    trimZeroes();
  }
}

void BigReal::init(INT64 n) {
  initToZero();
  if(n) {
    UINT64 un;
    if(n < 0) {
      flipSign();
      un = -n;
    } else {
      un = n;
    }
    m_expo = -1;
    do {
      insertDigit((BRDigitType)(un % BIGREALBASE));
      un /= BIGREALBASE;
      m_expo++;
    } while(un);
    trimZeroes();
  }
}

void BigReal::init(UINT64 n) {
  initToZero();
  if(n) {
    m_expo = -1;
    do {
      insertDigit((BRDigitType)(n % BIGREALBASE));
      n /= BIGREALBASE;
      m_expo++;
    } while(n);
    trimZeroes();
  }
}

void BigReal::init(const _int128 &n) {
  initToZero();
  if(n) {
    _uint128 un;
    if(n < 0) {
      flipSign();
      un = -n;
    } else {
      un = n;
    }
    m_expo = -1;
    do {
      insertDigit((BRDigitType)(un % BIGREALBASE));
      un /= BIGREALBASE;
      m_expo++;
    } while(un);
    trimZeroes();
  }
}

void BigReal::init(_uint128 n) {
  initToZero();
  if(n) {
    m_expo = -1;
    do {
      insertDigit((BRDigitType)(n % BIGREALBASE));
      n /= BIGREALBASE;
      m_expo++;
    } while(n);
    trimZeroes();
  }
}

BigReal::BigReal(const BigReal &x, DigitPool *digitPool) : m_digitPool(digitPool?*digitPool:x.m_digitPool) {
  startInit();
  initToZero();
  if(x._isnormal()) copyAllDigits(x);
  copyNonPointerFields(x);
  endInit();
}

BigReal &BigReal::operator=(const BigReal &x) {
  CHECKISMUTABLE(*this);
  if(&x == this) {
    return *this;
  } else if(!x._isnormal()) {
    return setToNonNormal(x.m_low, x.isNegative());
  }
  if(!_isnormal()) {  // x is normal
    copyAllDigits(x);
  } else {                   // both x and this are normal (have digits)
    const intptr_t missing = x.getLength() - getLength();
    if(missing >= 0) {       // First copy all the digits, then append the rest if x.length > this.length
      Digit *sd, *dd;
      for(sd = x.m_first, dd = m_first; dd; sd = sd->next, dd = dd->next) {
        dd->n = sd->n;
      }
      if(sd) {               // We have more digits to copy
        Digit *p;
        for(p = m_last; sd; sd = sd->next, p = dd) {
          (dd = newDigit())->prev = p;
          dd->n   = sd->n;
          p->next = dd;
        }
        (m_last = p)->next = NULL;
      }
    } else {                 // Missing < 0. First copy and then delete excess digits
      Digit *sd, *dd;
      for(sd = x.m_first, dd = m_first; sd; sd = sd->next, dd = dd->next) {
        dd->n = sd->n;
      }
      Digit *saveLast = m_last;
      m_last = dd->prev;
      m_last->next = NULL;
      deleteDigits(dd, saveLast);
    }
  }
  copyNonPointerFields(x);
  return *this;
}

void BigReal::init(const String &s, bool allowDecimalPoint) {
  String tmpstr(s);
  TCHAR *cp           = tmpstr.cstr();
  TCHAR *commaPos     = NULL;
  TCHAR *firstNonZero = NULL;
  TCHAR *lastNonZero  = NULL;
  bool negative       = false;

  initToZero();
  while(_istspace(*cp)) {
    cp++;
  }
  if(*cp == _T('-')) {
    negative = true; // Wait assigning the sign. We have to add fractional part
    cp++;
  } else if(*cp == _T('+')) {
    cp++;
  }

  TCHAR *t;
  if(allowDecimalPoint) {
    for(t = cp; _istdigit(*t) || *t == _T('.'); t++) {
      if(*t == _T('.')) {
        if(commaPos != NULL) { // We already got one
          *t = 0;
          break;
        }
        commaPos = t;
      } else if(*t != _T('0')) {
        if(firstNonZero == NULL) {
          firstNonZero = t;
        }
        lastNonZero = t;
      }
    }
  } else {
    for(t = cp; _istdigit(*t); t++) {
      if(*t != _T('0')) {
        if(firstNonZero == NULL) {
          firstNonZero = t;
        }
        lastNonZero = t;
      }
    }
  }

  if(firstNonZero != NULL) { // value != 0
    BigReal integerPart(   &m_digitPool);
    BigReal fractionalPart(&m_digitPool);
    TCHAR *exponentPos = t;
    if(commaPos == NULL) { // No decimalpoint. Assumed to be at the end.
      commaPos = t;
    }

    if(firstNonZero < commaPos) { // We got something before decimalpoint
      integerPart.clrInitDone();
      BRDigitType n      = 0;
      BRDigitType factor = 1;
      for(t = commaPos - 1; t >= firstNonZero; t--) {
        n += factor * (*t - _T('0'));
        factor *= 10;
        if(factor == BIGREALBASE) {
          integerPart.insertDigit(n);
          integerPart.incrExpo();
          n      = 0 ;
          factor = 1;
        }
      }
      if(n) { // Remember the last, if any
        integerPart.insertDigit(n);
        integerPart.incrExpo();
      }
      integerPart.trimZeroes().setInitDone();
    }

    if(lastNonZero > commaPos) { // We got something after the decimalpoint
      fractionalPart.clrInitDone();
      fractionalPart.m_expo = -1;
      fractionalPart.m_low  = 0;
      BRDigitType n = 0;
      if(*commaPos == _T('.')) {
        int count = 0;
        for(t = commaPos + 1; _istdigit(*t); t++) {
          n = 10 * n + (*t - _T('0'));
          count++;
          if(count == BIGREAL_LOG10BASE) {
            fractionalPart.appendDigit(n);
            fractionalPart.m_low--;
            n     = 0;
            count = 0;
          }
        }
        if(n) { // Remember the last digit
          if(count < BIGREAL_LOG10BASE) { // Make it a whole Digit
            n *= pow10(BIGREAL_LOG10BASE - count);
          }
          fractionalPart.appendDigit(n);
          fractionalPart.m_low--;
        }
      }
      fractionalPart.trimZeroes().setInitDone();
    }

    if(allowDecimalPoint && !fractionalPart.isZero()) {
      *this = integerPart + fractionalPart;
    } else {
      *this = integerPart;
    }

    if(negative) {
      flipSign();
    }

    if(*exponentPos == _T('e') || *exponentPos == _T('E')) {
      t = exponentPos + 1;
      int exponentSign = 1;
      if(*t == _T('-')) {
        exponentSign = -1;
        t++;
      } else if(*t == _T('+')) {
        t++;
      }
      BRExpoType exponent;
      for(exponent = 0; _istdigit(*t); t++) {
        exponent = 10 * exponent + (*t - '0');
        if(exponent > BIGREAL_LOG10BASE*BIGREAL_MAXEXPO || exponent < 0) { // max. exponent in base 10
          throwBigRealException(_T("Exponent too big. Max:%s"),format1000(BIGREAL_LOG10BASE*BIGREAL_MAXEXPO).cstr());
        }
      }
      exponent *= exponentSign;
      if(exponent != 0) {
        multPow10(exponent);
      }
    }
  }
}
