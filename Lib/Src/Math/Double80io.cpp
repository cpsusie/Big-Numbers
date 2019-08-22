#include "pch.h"
#include <ctype.h>
#include <Math/Double80.h>

using namespace std;

String toString(const Double80 &x, StreamSize precision, StreamSize width, FormatFlags flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return (String)stream;
}

static TCHAR *findFirstDigit(TCHAR *str) {
  for(;*str; str++) {
    if(_istdigit(*str)) return str;
  }
  return NULL;
}

#define MAXPRECISION DBL80_DIG

// Assume str consists of characters [0-9]
// return true if carry, false if no carry, ie. str=='9995" and lastDigitPos==2, will turn str into "000" and return true
static bool round5DigitString(String &str, intptr_t lastDigitPos) {
  bool         carry        = false;
  const size_t wantedLength = lastDigitPos + 1;
  if(wantedLength < str.length()) {
    if(str[wantedLength] >= '5') {
      TCHAR *first = str.cstr(), *dp = first + lastDigitPos;
      for(; dp >= first; dp--) {
        if(*dp < _T('9')) {
          (*dp)++;
          break;
        } else {
          *dp = '0';
        }
      }
      carry = (dp < first);
    }
    str.remove(wantedLength, str.length() - wantedLength);
  }
  return carry;
}

// Assume x != 0 and finite. ignore sign of x
static void formatFixed(String &result, const Double80 &x, intptr_t precision, FormatFlags flags, int expo10, bool removeTrailingZeroes) {
  TCHAR tmp[50];
#ifdef _DEBUG
  memset(tmp, 0, sizeof(tmp));
#endif
  d80tot(tmp, x);
  const TCHAR  *mantissa  = findFirstDigit(tmp);
  TCHAR        *comma     = _tcschr(tmp,_T('.'));
  TCHAR        *decimals  = NULL;
  TCHAR        *estr      = _tcschr(tmp,_T('e'));
  int           e10       = 0;

  if(comma != NULL) {
    *(comma++) = 0;
    decimals = comma;
  }
  if(estr != NULL) {
    *(estr++) = 0;
    e10 = _ttoi(estr);
  }
  String   ciphers  = mantissa;
  intptr_t commaPos = ciphers.length();
  if(decimals != NULL) {
    ciphers += decimals;
  }
  commaPos += e10;
  intptr_t lastResultDigitPos = commaPos + precision - 1;
  if(commaPos <= 0) { // leading zeroes
    const intptr_t zeroCount = 1-commaPos;
    ciphers.insert(0, zeroCount, '0');
    commaPos           += zeroCount;
    lastResultDigitPos += zeroCount;
    if(round5DigitString(ciphers, lastResultDigitPos)) {
      ciphers.insert(0, '1'); // carry propagated all the way up
      commaPos++;
    }
    result += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      StrStream::addDecimalPoint(result);
      if(precision > 0) {
        result += substr(ciphers, commaPos, precision);
      }
      if(removeTrailingZeroes) StrStream::removeTralingZeroDigits(result);
    }
  } else if(commaPos > (intptr_t)ciphers.length()) {
    const intptr_t zeroCount = commaPos - (intptr_t)ciphers.length();
    result += ciphers;
    StrStream::addZeroes(result, zeroCount);
    if((flags & ios::showpoint) || (precision > 0)) {
      StrStream::addDecimalPoint(result);
      if(!removeTrailingZeroes && (precision > 0)) {
        StrStream::addZeroes(result, precision);
      }
    }
  } else { // 0 < commaPos <= ciphers.length()
    if(round5DigitString(ciphers, lastResultDigitPos)) {
      ciphers.insert(0, '1'); // carry propagated all the way up
      commaPos++;
    }
    result += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      StrStream::addDecimalPoint(result);
      if(precision > 0) {
        result += substr(ciphers, commaPos, (intptr_t)precision);
        if(removeTrailingZeroes) {
          StrStream::removeTralingZeroDigits(result);
        } else {
          const intptr_t zeroCount = precision - ((intptr_t)ciphers.length() - commaPos);
          if(zeroCount > 0) {
            StrStream::addZeroes(result, zeroCount);
          }
        }
      }
    }
  }
}

// Assume x != 0 and finite. ignore sign of x
static void formatScientific(String &result, const Double80 &x, intptr_t precision, FormatFlags flags, bool removeTrailingZeroes) {
  TCHAR tmp[50];
#ifdef _DEBUG
  memset(tmp, 0, sizeof(tmp));
#endif
  d80tot( tmp, x);
  TCHAR  *mantissa  = findFirstDigit(tmp);
  String  decimals;
  TCHAR  *comma     = _tcschr(tmp,_T('.'));
  TCHAR  *estr      = _tcschr(tmp,_T('e'));
  int     exponent  = 0;
  bool    exponentCharAdded = false;

  if(estr != NULL) {
    *(estr++) = 0;
    exponent = _ttoi(estr);
  }

  if(comma != NULL) {
    *(comma++) = 0;
    decimals = comma;
    if(precision+1 < (intptr_t)decimals.length()) {
      if(round5DigitString(decimals, precision-1)) {
        if(mantissa[0] < _T('9')) {
          mantissa[0]++;
        } else {
          mantissa[0] = _T('1');
          exponent++;
        }
      }
    }
    if(removeTrailingZeroes) {
      StrStream::removeTralingZeroDigits(decimals);
    } else {
      const intptr_t zeroCount = precision - decimals.length();
      if(zeroCount > 0) StrStream::addZeroes(decimals, zeroCount);
    }
  }
  result += mantissa;

  if((flags & ios::showpoint) || (decimals.length() > 0)) {
    StrStream::addDecimalPoint(result);
    if(decimals.length() > 0) {
      result += decimals;
    }
  }
  StrStream::addExponentChar(result, flags);
  result += format(_T("%+03d"), exponent);
}

// Assume x != 0 and finite. ignore sign of x
static void formatHex(String &result, const Double80 &x, UINT precision, FormatFlags flags) {
  if(precision == 0) {
    precision = 6;
  }
  int e2;
  if(x.isZero()) {
    e2 = 0;
  } else {
    e2 = getExpo2(x);
    if(e2 < numeric_limits<Double80>::min_exponent) {
      e2 = numeric_limits<Double80>::min_exponent;
    }
  }
  // precision >= 1!!
  UINT64 sig       = getSignificand(x);
  UINT   intPart   = sig >> 63;
  UINT   zeroCount = 0;
  TCHAR  fractionStr[100];
  sig <<= 1; // to get the fraction part only
  if(precision >= 16) {
    _stprintf(fractionStr, _T("%016I64x"), sig);
    zeroCount = precision - 16;
  } else { // 0 < precision < 16.    => do rounding
    const UINT shift = (15 - precision) * 4;
    if(shift) sig >>= shift;
    const BYTE lastDigit = sig & 0xf;
    sig >>= 4;
    if(lastDigit > 8) { // round up
      sig++;
      const UINT64 intPartMask = 1ui64 << (60 - shift);
      if(sig & intPartMask) {
        intPart++;
        sig &= ~intPartMask;
      }
    }
    _stprintf(fractionStr, _T("%0*I64x"), precision, sig);
  }
  if(flags&ios::uppercase) {
    strToUpperCase(fractionStr);
  }
  StrStream::addHexPrefix(result, flags);
  result += (_T('0') + intPart);
  StrStream::addDecimalPoint(result);
  result += fractionStr;
  if(zeroCount) {
    StrStream::addZeroes(result, zeroCount);
  }
  StrStream::addHexExponentChar(result, flags);
  result += format(_T("%+d"), e2);
}

StrStream &operator<<(StrStream &stream, const Double80 &x) {
  intptr_t          precision  = (intptr_t)stream.getPrecision();
  const FormatFlags flags      = stream.getFlags();

  if(precision < 0) {
    precision = MAXPRECISION;
  }

  String result;
  if(!isfinite(x)) {
    result = StrStream::formatUndefined(x, (flags & ios::uppercase));
  } else { // x defined
    if(x.isNegative()) {
      result = _T("-");
    } else if(flags & ios::showpos) {
      result = _T("+");
    }
    if(x.isZero()) {
      StrStream::formatZero(result, precision, flags, MAXPRECISION);
    } else { // x defined && x != 0
      const int expo10 = Double80::getExpo10(x);
      switch(flags & ios::floatfield) {
      case 0              : // No float-format is specified. Format depends on e10 and precision
        if((expo10 < -4) || (expo10 > 14) || ((expo10 > 0) && (expo10 >= precision)) || (expo10 > precision)) {
          precision = max(0,precision-1);
          formatScientific(result, x, precision, flags, (flags & ios::showpoint) == 0);
        } else {
          const intptr_t prec = (precision == 0) ? abs(expo10) : max(0,precision-expo10-1);
          formatFixed(result, x, prec, flags, expo10, ((flags & ios::showpoint) == 0) || precision <= 1);
        }
        break;
      case ios::scientific: // Use scientific format
        formatScientific(result, x, precision, flags, false);
        break;
      case ios::fixed     : // Use fixed format
        formatFixed(result, x, precision, flags, expo10, false);
        break;
      case ios::hexfloat  :
        formatHex(result, x, (UINT)max(0,precision), flags);
        break;
      }
    } // x defined && x != 0
  } // end x defined

  return stream.appendFilledField(result, flags);
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Double80 &x) {
  StrStream stream(out);
  stream << x;
  USES_ACONVERSION;
  out << T2A(stream.cstr());
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

#include "IStreamUtil.h"

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Double80 &x) {
  SETUPISTREAM(in)

  if(FloatingValueStreamScanner::getInstance().match(in, &buf) < 0) {
    ungetbuf(in);
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    return in;
  }
  try {
    x = _tcstod80(buf.cstr(),NULL);
  } catch(...) {
    RESTOREISTREAM(in);
    in.setstate(ios::failbit);
    throw;
  }
  RESTOREISTREAM(in);
  return in;
}

istream &operator>>(istream &in, Double80 &x) {
  return ::operator>> <istream,char>(in, x);
}

ostream &operator<<(ostream &out, const Double80 &x) {
  return ::operator<< <ostream>(out, x);
}

std::wistream &operator>>(std::wistream &in, Double80 &x) {
  return ::operator>> <std::wistream,wchar_t>(in, x);
}

std::wostream &operator<<(std::wostream &out, const Double80 &x) {
  return ::operator<< <std::wostream>(out, x);
}
