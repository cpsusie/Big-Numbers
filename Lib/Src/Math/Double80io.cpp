#include "pch.h"
#include <StrStream.h>
#include <Math/Double80.h>

using namespace std;

class D80StrStream : public StrStream {
private:
  static TCHAR *findFirstDigit(TCHAR *str);
  static bool   round5DigitString(String &str, intptr_t lastDigitPos);
  static void   formatFixed(      String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, int expo10, bool removeTrailingZeroes);
  static void   formatScientific( String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes);
  static void   formatHex(        String &dst, const Double80 &x, StreamSize precision, FormatFlags flags);
public:
  D80StrStream(const ostream &s) : StrStream(s) {
  }
  D80StrStream(const wostream &s) : StrStream(s) {
  }
  D80StrStream &operator<<(const Double80 &x);
};

TCHAR *D80StrStream::findFirstDigit(TCHAR *str) {
  for(;*str; str++) {
    if(_istdigit(*str)) return str;
  }
  return NULL;
}

// Assume str consists of characters [0-9]
// return true if carry, false if no carry, ie. str=='9995" and lastDigitPos==2, will turn str into "000" and return true
bool D80StrStream::round5DigitString(String &str, intptr_t lastDigitPos) {
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
void D80StrStream::formatFixed(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, int expo10, bool removeTrailingZeroes) {
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
}

// Assume x != 0 and finite. ignore sign of x
void D80StrStream::formatScientific(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes) {
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
}

// Assume x != 0 and finite and precision >= 0. ignore sign of x
void D80StrStream::formatHex(String &dst, const Double80 &x, StreamSize precision, FormatFlags flags) {
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
}

#define MAXPRECISION numeric_limits<Double80>::digits10

D80StrStream &D80StrStream::operator<<(const Double80 &x) {
  StreamSize  prec = precision();
  FormatFlags flg  = flags();

  if(prec < 0) {
    prec = MAXPRECISION;
  }

  String result;
  if(!isfinite(x)) {
    TCHAR tmp[100];
    result = formatUndefined(tmp, _fpclass(x), (flg & ios::uppercase), true);
    flg &= ~ios::hexfloat;
  } else if(x.isZero()) {
    formatZero(result, prec, flg, MAXPRECISION);
  } else { // x defined && x != 0
    const int expo10 = Double80::getExpo10(x);
    switch(flg & ios::floatfield) {
    case 0              : // No float-format is specified. Format depends on e10 and precision
      if((expo10 < -4) || (expo10 > 14) || ((expo10 > 0) && (expo10 >= prec)) || (expo10 > prec)) {
        prec = max(0,prec-1);
        formatScientific(result, x, prec, flg, (flg & ios::showpoint) == 0);
      } else {
        const StreamSize prec1 = (prec == 0) ? abs(expo10) : max(0,prec-expo10-1);
        formatFixed(result, x, prec1, flg, expo10, ((flg & ios::showpoint) == 0) || prec <= 1);
      }
      break;
    case ios::scientific: // Use scientific format
      if(prec == 0) prec = 6;
      formatScientific(result, x, prec, flg, false);
      break;
    case ios::fixed     : // Use fixed format
      formatFixed(result, x, prec, flg, expo10, false);
      break;
    case ios::hexfloat  :
      formatHex(result, x, prec, flg);
      break;
    }
  } // x defined && x != 0
  formatFilledFloatField(result, x.isNegative(), flg);
  return *this;
}

template <class OStreamType> OStreamType &putDouble80(OStreamType &out, const Double80 &x) {
  D80StrStream stream(out);
  stream << x;
  out << (String&)stream;
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

template <class IStreamType, class CharType> IStreamType &getDouble80(IStreamType &in, Double80 &x) {
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

String toString(const Double80 &x, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << x).str().c_str();
}
