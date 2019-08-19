#include "pch.h"
#include <String.h>
#include <ctype.h>

using namespace std;

TCHAR BigRealStream::setSpaceChar(TCHAR value) {
  TCHAR ospac = m_spaceChar;
  m_spaceChar = value;
  return ospac;
}

#define addDecimalPoint(s)       { if(!decimalPointAdded) { s += _T("."); decimalPointAdded = true; } }
#define addExponentChar(s)       { s += ((flags & ios::uppercase) ? _T("E") : _T("e"));               }
#define addZeroes(      s,count) { s.insert(s.length(),(count), '0');                                 }

static void removeTralingZeroDigits(String &str) {
  while(str.last() == '0') {
    str.removeLast();
  }
  if(str.last() == '.') {
    str.removeLast();
  }
}

static TCHAR *digitToStr(TCHAR *dst, BRDigitType n, UINT width = 0) {
  TCHAR tmp[50], *d = width ? tmp : dst;
#ifdef IS64BIT
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

void BigReal::formatFixed(String &result, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes) const {
  bool decimalPointAdded = false;

  const BigReal nn(round(*this, (intptr_t)precision));
  if(nn.isZero()) {
    StrStream::formatZero(result, precision, flags);
    return;
  }

  const      Digit *digit        = nn.m_first;
  BRExpoType        d            = getExpo10(nn);
  int               decimalsDone = 0;
  TCHAR             digStr[100];

  if(d < 0) { // first handle integerpart
    result += _T("0");
  } else {
    result += digitToStr(digStr, digit->n);
    d -= BigReal::getDecimalDigitCount(digit->n);
    for(digit = digit->next; digit != NULL && (d >= 0); d -= LOG10_BIGREALBASE, digit = digit->next) {
      result += digitToStr(digStr, digit->n, LOG10_BIGREALBASE);
    }
    addZeroes(result,d + 1);
    d = -1;
  }

  // now handle fraction if wanted
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(result);

    if(precision > 0) {
      for(int i = 0; (i - LOG10_BIGREALBASE > d) && (decimalsDone < precision); i -= LOG10_BIGREALBASE) {
        int      partLength = LOG10_BIGREALBASE;
        intptr_t rest       = (intptr_t)(precision - decimalsDone);
        if(rest < LOG10_BIGREALBASE) {
          partLength = (int)rest;
        }
        result += digitToStr(digStr, 0, partLength);
        decimalsDone += partLength;
      }
      for(; digit && (decimalsDone < precision); digit = digit->next) {
        BRDigitType part       = digit->n;
        int         partLength = LOG10_BIGREALBASE;
        intptr_t    rest       = (intptr_t)(precision - decimalsDone);
        if(rest < LOG10_BIGREALBASE) {
          partLength = (int)rest;
          part /= BigReal::pow10(LOG10_BIGREALBASE - (int)rest);
        }
        result += digitToStr(digStr, part, partLength);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && (precision > decimalsDone)) {
        addZeroes(result,precision - decimalsDone);
      }
      if(removeTrailingZeroes) {
        removeTralingZeroDigits(result);
      }
    }
  }
}

void BigReal::formatScientific(String &result, StreamSize precision, FormatFlags flags, BRExpoType expo10, bool removeTrailingZeroes) const {
  bool decimalPointAdded = false;
  bool exponentCharAdded = false;

  const BigReal nn(round(*this, (intptr_t)(precision - expo10)));
  const Digit *digit = nn.m_first;
  int          decimalsDone = 0;
  int          scale = getDecimalDigitCount(digit->n) - 1;
  BRDigitType  scaleE10 = pow10(scale);

  TCHAR digStr[100];
  result += digitToStr(digStr, digit->n / scaleE10);
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(result);

    if(precision > 0) {
      BRDigitType fraction = digit->n % scaleE10;
      if(precision < scale) {
        fraction /= pow10((int)(scale - precision));
        decimalsDone = (int)precision; // precision < scale < LOG10_BIGREALBASE
      } else {
        decimalsDone = (int)scale;     // scale < LOG10_BIGREALBASE
      }
      if(decimalsDone > 0) {           // decimalsDone < LOG10_BIGREALBASE
        result += digitToStr(digStr, fraction, decimalsDone);
      }
      for(digit = digit->next; digit != NULL && decimalsDone < precision; digit = digit->next) { // now handle tail
        BRDigitType part = digit->n;
        int         partLength = LOG10_BIGREALBASE;
        intptr_t    rest = (intptr_t)(precision - decimalsDone);
        if(rest < LOG10_BIGREALBASE) {
          partLength = (int)rest;
          part /= pow10(LOG10_BIGREALBASE - (int)rest);
        }
        result += digitToStr(digStr, part, partLength);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && decimalsDone < precision) {
        addZeroes(result, precision - decimalsDone);
      }
      if(removeTrailingZeroes) {
        removeTralingZeroDigits(result);
      }
    }
  }
  addExponentChar(result);
#ifdef IS32BIT
  result += format(_T("%+03d"), BigReal::getExpo10(nn));
#else
  result += format(_T("%+03lld"), BigReal::getExpo10(nn));
#endif
}

void BigReal::formatWithSpaceChar(String &result, TCHAR spaceChar) const {
  if(isZero()) {
    result += _T("0");
  } else {
    bool decimalPointAdded = false;
    BRExpoType d = m_expo;
    if(d < 0) {
      result += _T("0");
      addDecimalPoint(result);
      for(int i = -1; i > d; i--) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, 0, spaceChar);
      }
      for(const Digit *digit = m_first; digit; digit = digit->next) {
#ifdef IS32BIT
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, digit->n, spaceChar);
#else
        result += format(_T("%0*.*llu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, digit->n, spaceChar);
#endif
      }
    } else {
#ifdef IS32BIT
      result += format(_T("%lu%c"), m_first->n, spaceChar);
#else
      result += format(_T("%llu%c"), m_first->n, spaceChar);
#endif
      if(d-- == 0 && m_first->next) {
        addDecimalPoint(result);
      }
      for(Digit *digit = m_first->next; digit; digit = digit->next) {
#ifdef IS32BIT
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, digit->n, spaceChar);
#else
        result += format(_T("%0*.*llu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, digit->n, spaceChar);
#endif
        if(d-- == 0 && digit->next)
          addDecimalPoint(result);
      }
      for(; d >= 0; d--) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE, LOG10_BIGREALBASE, 0, spaceChar);
      }
    }
    if(result.last() == spaceChar) {
      result.removeLast();
    }
  }
}

BigRealStream &operator<<(BigRealStream &stream, const BigReal &x) {
  intptr_t          precision = (intptr_t)stream.getPrecision();
  const FormatFlags flags     = stream.getFlags();
  const TCHAR       spaceChar = stream.getSpaceChar();

  String result;
  if(!isfinite(x)) {
    result = StrStream::formatUndefined(x);
  } else {
    if(x.isNegative()) {
      result += _T("-");
    } else if(flags & ios::showpos) {
      result += _T("+");
    }

    if(spaceChar != 0) {
      x.formatWithSpaceChar(result, spaceChar);
    } else {
      if(x.isZero()) {
        StrStream::formatZero(result, precision, flags);
      } else { // x defined && x != 0
        if((flags & ios::floatfield) == ios::fixed) { // Use fixed format
          x.formatFixed(result, precision, flags, false);
        } else {
          BRExpoType expo10 = BigReal::getExpo10(x);
          if((flags & ios::floatfield) == ios::scientific) { // Use scientific format
            x.formatScientific(result, precision, flags, expo10, false);
          } else if(expo10 < -4 || expo10 > 14 || (expo10 > 0 && expo10 >= precision) || expo10 > precision) { // neither scientific nor fixed format (or both) are specified
            precision = max(0, precision - 1);
            x.formatScientific(result, precision, flags, expo10, (flags & ios::showpoint) == 0);
          } else {
            const intptr_t prec = (precision == 0) ? abs(expo10) : max(0, precision - expo10 - 1);
            x.formatFixed(result, prec, flags, ((flags & ios::showpoint) == 0) || precision <= 1);
          }
        }
      } // x defined && x != 0
    }
  }
  stream.appendFilledField(result, flags);
  return stream;
}

BigRealStream &operator<<(BigRealStream &out, const FullFormatBigReal &n) {
  if(isfinite(n)) {
    BRExpoType e = BigReal::getExpo10(n);
    intptr_t   precision;
    if(e < 0) {
      precision = n.getLength() * LOG10_BIGREALBASE;
      if(e % LOG10_BIGREALBASE == 0) {
        precision -= LOG10_BIGREALBASE;
      } else {
        precision -= -e % LOG10_BIGREALBASE;
      }
    } else {
      precision = (n.getLength() - 1) * LOG10_BIGREALBASE + (e % LOG10_BIGREALBASE);
    }

    if(n.getLength() > 1) {
      for(BRDigitType last = n.getLastDigit(); last % 10 == 0; last /= 10) precision--;
    }
    out.setFlags((out.getFlags() | ios::scientific) & ~ios::fixed);
    out.setPrecision(precision);
  }
  out << (const BigReal&)n;
  return out;
}

String FullFormatBigReal::toString(bool spacing) const {
  BigRealStream stream;
  if(spacing) stream.setSpaceChar(_T(':'));
  stream << *this;
  return (String)stream;
}

BigRealStream &operator<<(BigRealStream &out, const BigInt &n) {
  out.setFlags((out.getFlags() | ios::fixed) & ~ios::scientific);
  out.setPrecision(0);
  out << (const BigReal&)n;
  return out;
}

String BigReal::toString() const {
  return ::toString(*this);
}

String toString(const BigReal &n, StreamSize precision, StreamSize width, FormatFlags flags) {
  BigRealStream buf(precision, width, flags);
  buf << n;
  return buf;
}
