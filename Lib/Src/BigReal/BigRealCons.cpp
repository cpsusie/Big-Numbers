#include "pch.h"
#include <float.h>
#include <ctype.h>

#ifdef _DEBUG

void BigReal::ajourDebugString() const {
  _tcsncpy(m_debugString, ::toString((*this),ARRAYSIZE(m_debugString)-10,0).cstr(),ARRAYSIZE(m_debugString)-1);
  LASTVALUE(m_debugString) = 0;
}

#endif

bool BigReal::enableDebugString(bool enabled) { // static
#ifndef _DEBUG
  return false;
#else
  if(!getDebuggerPresent()) {
    return false;
  } else {
    s_debugStringGate.wait();
    const bool result = s_debugStringEnabled;
    s_debugStringEnabled = enabled;
    s_debugStringGate.signal();
    return result;
  }
#endif
}

bool BigReal::isDebugStringEnabled() { // static
#ifdef _DEBUG
  return s_debugStringEnabled;
#else
  return false;
#endif
}

void BigReal::init(int n) {
  init();
  if(n) {
    BRDigitType un;
    if(n < 0) {
      m_negative = true;
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

void BigReal::init(unsigned int n) {
  init();
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

void BigReal::init(__int64 n) {
  init();
  if(n) {
    unsigned __int64 un;
    if(n < 0) {
      m_negative = true;
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

void BigReal::init(unsigned __int64 n) {
  init();
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
  init();
  m_expo     = x.m_expo;
  m_low      = x.m_low;
  m_negative = x.m_negative;
  if(!x.isZero()) copyAllDigits(x);
  COPYBIGREALDEBUGSTRING(x);
}

BigReal &BigReal::operator=(const BigReal &x) {
  if(&x != this) {
    if(x.isZero()) {
      if(!isZero()) {
        setToZero();
      }
      return *this;
    } else if(isZero()) {      // x != 0
      copyAllDigits(x);
    } else {                   // x != 0 && *this != 0
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
        m_digitPool.deleteDigits(dd, saveLast);
      }
    }
    m_expo     = x.m_expo;
    m_low      = x.m_low;
    m_negative = x.m_negative;
    COPYBIGREALDEBUGSTRING(x)
  }
  return *this;
}

void BigReal::init(const String &s, bool allowDecimalPoint) {
  ENTER_CRITICAL_SECTION_BIGREAL_DEBUGSTRING();

  String tmpstr(s);
  TCHAR *cp           = tmpstr.cstr();
  TCHAR *commaPos     = NULL;
  TCHAR *firstNonZero = NULL;
  TCHAR *lastNonZero  = NULL;
  bool negative       = false;

  init();
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
      integerPart.trimZeroes();
    }

    if(lastNonZero > commaPos) { // We got something after the decimalpoint
      fractionalPart.m_expo = -1;
      fractionalPart.m_low  = 0;
      BRDigitType n = 0;
      if(*commaPos == _T('.')) {
        int count = 0;
        for(t = commaPos + 1; _istdigit(*t); t++) {
          n = 10 * n + (*t - _T('0'));
          count++;
          if(count == LOG10_BIGREALBASE) {
            fractionalPart.appendDigit(n);
            fractionalPart.m_low--;
            n     = 0;
            count = 0;
          }
        }
        if(n) { // Remember the last digit
          if(count < LOG10_BIGREALBASE) { // Make it a whole Digit
            n *= pow10(LOG10_BIGREALBASE - count);
          }
          fractionalPart.appendDigit(n);
          fractionalPart.m_low--;
        }
      }
      fractionalPart.trimZeroes();
    }

    if(allowDecimalPoint && !fractionalPart.isZero()) {
      *this = integerPart + fractionalPart;
    } else {
      *this = integerPart;
    }

    if(negative) {
      m_negative = true;
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
        if(exponent > LOG10_BIGREALBASE*BIGREAL_MAXEXPO || exponent < 0) { // max. exponent in base 10
          throwBigRealException(_T("Exponent too big. Max:%s"),format1000(LOG10_BIGREALBASE*BIGREAL_MAXEXPO).cstr());
        }
      }
      exponent *= exponentSign;
      if(exponent != 0) {
        multPow10(exponent);
      }
    }
  }
  LEAVE_CRITICAL_SECTION_BIGREAL_DEBUGSTRING(ajourDebugString(););
}
