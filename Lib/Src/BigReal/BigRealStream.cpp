#include "pch.h"
#include "BigRealStream.h"

static TCHAR *digitToStr(TCHAR *dst, BRDigitType n, UINT width = 0) {
  TCHAR tmp[50], *d = width ? tmp : dst;
#if defined(IS64BIT)
  _i64tot(n, d, 10);
#else
  _itot(n, d, 10);
#endif // IS53BIT
  if(width) {
    const int zeroCount = (int)width - (int)_tcslen(tmp);
    if(zeroCount <= 0) {
      _tcscpy(dst, d);
    } else { // zeroCount > 0
      TMEMSET(dst, _T('0'), zeroCount);
      _tcscpy(dst + zeroCount, d);
    }
  }
  return dst;
}

class BigRealStringFields: public FloatStringFields {
public:
  // Assume x._isnormal()
  BigRealStringFields(const BigReal &x, size_t maxDecimalDigitCount);
};

BigRealStringFields::BigRealStringFields(const BigReal &x, size_t maxDecimalDigitCount) : FloatStringFields(2) {
  assert(x._isnormal());
  const      Digit *digit = x.m_first;
  UINT              decimalsDone = 0;
  TCHAR             digStr[100];

  digitToStr(digStr, digit->n);
  m_ciphers = digStr;
  decimalsDone = m_ciphers.length();
  for(digit = digit->next; digit && (decimalsDone < maxDecimalDigitCount); decimalsDone += BIGREAL_LOG10BASE, digit = digit->next) {
    m_decimals += digitToStr(digStr, digit->n, BIGREAL_LOG10BASE);
  }
  m_expo10 = x.m_expo * BIGREAL_LOG10BASE;
  normalize();
}

static String &formatSeparateDigits(String &dst, const BigReal &x, TCHAR separatorChar) {
  dst = format(_T("%s not implemented"), __TFUNCTION__);
  return dst;
}

size_t BigRealStreamParameters::findMaxDecimalDigitCount(const BigReal &x) const {
  assert(x._isnormal());
  const size_t      len    = x.getDecimalDigits();
  const FormatFlags flgs   = flags();
  const BRExpoType  expo10 = BigReal::getExpo10(x);
  StreamSize        prec   = precision();
  if(prec < 0) prec = 0;
  switch(flgs & ios::floatfield) {
  case 0              :
    if((flgs & ios::showpoint) && (prec == 0)) prec = 6;
    if(prec > 0) {
      return prec + 2;
    }
    if((expo10 < -4) || (expo10 >= 6)) {
      return 8; // scientific
    } else {
      return (expo10 >= 0) ? expo10 + 7 : 8; // fixed
    }
  case ios::scientific: // Use scientific format
    return prec ? prec+2 : 8;
  case ios::fixed     : // Use fixed format
    return max(2, expo10 + prec + 2);
  }
  return 0;
}

String &BigRealStreamParameters::getFormattedString(String &dst, const BigReal &x) {
  intptr_t    prec          = (intptr_t)precision();
  FormatFlags flgs          = flags();
  const TCHAR separatorChar = separator();

  if((flgs & ios::floatfield) == ios::hexfloat) {
    flgs &= ~ios::fixed;
    flags(flgs);
  }
  String brStr;
  if(!isfinite(x)) {
    TCHAR tmp[100];
    brStr = formatUndefined(tmp, _fpclass(x), (flgs & ios::uppercase), true);
    flgs &= ~ios::hexfloat;
    flags(flgs);
  } else if(separatorChar != 0) {
    formatSeparateDigits(brStr, x, separatorChar);
  } else if(x.isZero()) {
    formatZero(brStr, prec, flgs);
  } else { // x defined && x != 0
    BigRealStringFields(x, findMaxDecimalDigitCount(x)).formatFloat(brStr, prec, flgs);
  } // x defined && x != 0
  return formatFilledFloatField(dst, brStr, x.isNegative(), *this);
}
