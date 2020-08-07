#include "pch.h"
#include <StrStream.h>
#include <Math/Double80.h>

using namespace std;
using namespace OStreamHelper;
using namespace IStreamHelper;

template<typename CharType> CharType *findFirstDigit(CharType *str) {
  for(;*str; str++) {
    if(iswdigit(*str)) return str;
  }
  return NULL;
}

// Assume str consists of characters [0-9]
// Return true if carry, false if no carry, ie. str=='9995" and lastDigitPos==2, will turn str into "000" and return true
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
// Return dst
static String &formatFixed(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, int expo10, bool removeTrailingZeroes) {
  TCHAR tmp[50];
#if defined(_DEBUG)
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
  String ciphers  = mantissa;
  int    commaPos = (int)ciphers.length();
  if(decimals != NULL) {
    ciphers += decimals;
  }
  commaPos += e10;
  int lastResultDigitPos = commaPos + (int)precision - 1;
  if(commaPos <= 0) { // leading zeroes
    const int zeroCount = 1-commaPos;
    ciphers.insert(0, zeroCount, '0');
    commaPos           += zeroCount;
    lastResultDigitPos += zeroCount;
    if(round5DigitString(ciphers, lastResultDigitPos)) {
      ciphers.insert(0, '1'); // carry propagated all the way up
      commaPos++;
    }
    dst += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(dst);
      if(precision > 0) {
        dst += substr(ciphers, commaPos, (intptr_t)precision);
      }
      if(removeTrailingZeroes) removeTralingZeroDigits(dst);
    }
  } else if(commaPos > (int)ciphers.length()) {
    const int zeroCount = commaPos - (int)ciphers.length();
    dst += ciphers;
    addZeroes(dst, zeroCount);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(dst);
      if(!removeTrailingZeroes && (precision > 0)) {
        addZeroes(dst, (size_t)precision);
      }
    }
  } else { // 0 < commaPos <= ciphers.length()
    if(round5DigitString(ciphers, lastResultDigitPos)) {
      ciphers.insert(0, '1'); // carry propagated all the way up
      commaPos++;
    }
    dst += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(dst);
      if(precision > 0) {
        dst += substr(ciphers, commaPos, (intptr_t)precision);
        if(removeTrailingZeroes) {
          removeTralingZeroDigits(dst);
        } else {
          const int zeroCount = (int)precision - ((int)ciphers.length() - commaPos);
          if(zeroCount > 0) {
            addZeroes(dst, zeroCount);
          }
        }
      }
    }
  }
  return dst;
}

// Assume x != 0 and finite. ignore sign of x
// Return dst
static String &formatScientific(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes) {
  TCHAR tmp[50];
#if defined(_DEBUG)
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
      if(round5DigitString(decimals, (intptr_t)precision-1)) {
        if(mantissa[0] < _T('9')) {
          mantissa[0]++;
        } else {
          mantissa[0] = _T('1');
          exponent++;
        }
      }
    }
    if(removeTrailingZeroes) {
      removeTralingZeroDigits(decimals);
    } else {
      const int zeroCount = (int)precision - (int)decimals.length();
      if(zeroCount > 0) addZeroes(decimals, zeroCount);
    }
  }
  dst += mantissa;

  if((flags & ios::showpoint) || (decimals.length() > 0)) {
    addDecimalPoint(dst);
    if(decimals.length() > 0) {
      dst += decimals;
    }
  }
  addExponentChar(dst, flags);
  dst += format(_T("%+03d"), exponent);
  return dst;
}

// Assume x != 0 and finite and precision >= 0. ignore sign of x
// Return dst
static String &formatHex(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags) {
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
  size_t zeroCount = 0;
  TCHAR  fractionStr[100];
  sig <<= 1; // to get the fraction part only
  if(precision >= 16) {
    _stprintf(fractionStr, _T("%016I64x"), sig);
    zeroCount = (size_t)precision - 16;
  } else { // 0 < precision < 16.    => do rounding
    const UINT shift = (UINT)(15 - precision) * 4;
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
    _stprintf(fractionStr, _T("%0*I64x"), (int)precision, sig);
  }
  if(flags&ios::uppercase) {
    strToUpperCase(fractionStr);
  }
  dst += (_T('0') + intPart);
  addDecimalPoint(dst);
  dst += fractionStr;
  if(zeroCount) {
    addZeroes(dst, zeroCount);
  }
  addExponentChar(dst, flags);
  dst += format(_T("%+d"), e2);
  return dst;
}

#define MAXPRECISION numeric_limits<Double80>::digits10

// Return dst
template<typename StringType> StringType &formatD80(StringType &dst, const Double80 &x, StreamParameters &param) {
  StreamSize  prec  = param.precision();
  FormatFlags flags = param.flags();

  if(prec < 0) {
    prec = MAXPRECISION;
  }

  String d80Str;
  if(!isfinite(x)) {
    TCHAR tmp[100];
    d80Str = formatUndefined(tmp, _fpclass(x), (flags & ios::uppercase), true);
    flags &= ~ios::hexfloat;
    param.flags(flags);
  } else if(x.isZero()) {
    formatZero(d80Str, prec, flags, MAXPRECISION);
  } else { // x defined && x != 0
    const int expo10 = Double80::getExpo10(x);
    switch(flags & ios::floatfield) {
    case 0              : // No float-format is specified. Format depends on e10 and precision
      if((expo10 < -4) || (expo10 > 14) || ((expo10 > 0) && (expo10 >= prec)) || (expo10 > prec)) {
        prec = max(0,prec-1);
        formatScientific(d80Str, x, prec, flags, (flags & ios::showpoint) == 0);
      } else {
        const StreamSize prec1 = (prec == 0) ? abs(expo10) : max(0,prec-expo10-1);
        formatFixed(d80Str, x, prec1, flags, expo10, ((flags & ios::showpoint) == 0) || prec <= 1);
      }
      break;
    case ios::scientific: // Use scientific format
      if(prec == 0) prec = 6;
      formatScientific(d80Str, x, prec, flags, false);
      break;
    case ios::fixed     : // Use fixed format
      formatFixed(d80Str, x, prec, flags, expo10, false);
      break;
    case ios::hexfloat  :
      formatHex(d80Str, x, prec, flags);
      break;
    }
  } // x defined && x != 0
  return formatFilledFloatField(dst, d80Str, x.isNegative(), param);
}

template<typename OStreamType> OStreamType &putDouble80(OStreamType &out, const Double80 &x) {
  String s;
  out << formatD80(s, x, StreamParameters(out));
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

template<typename IStreamType, typename CharType> IStreamType &getDouble80(IStreamType &in, Double80 &x) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const bool          hex   = ((in.flags() & ios::floatfield) == ios::hexfloat);
  const RegexIStream &regex = hex ? HexFloatValueStreamScanner::getInstance() : DecFloatValueStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    x = _tcstod80(buf.cstr(),NULL);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

istream &operator>>(istream &in, Double80 &x) {
  return getDouble80<istream,char>(in, x);
}

ostream &operator<<(ostream &out, const Double80 &x) {
  return putDouble80(out, x);
}

wistream &operator>>(wistream &in, Double80 &x) {
  return getDouble80<wistream,wchar_t>(in, x);
}

wostream &operator<<(wostream &out, const Double80 &x) {
  return putDouble80(out, x);
}
