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

#define addDecimalPoint(s)        { s += _T("."); }
#define addExponentChar(s)        { s += ((flags & ios::uppercase) ? _T("E") : _T("e")); }
#define addZeroes(      s, count) { s.insert(s.length(), (count), '0'); }

// Assume str consists of characters [0-9]
// return carry, 0 or 1
static UINT round5DigitString(String &str, intptr_t lastDigitPos) {
  if((lastDigitPos < (intptr_t)str.length()-1) && (str[lastDigitPos+1] >= '5')) {
    TCHAR *first = str.cstr(), *dp = first + lastDigitPos;
    for(; dp >= first; dp--) {
      if(*dp < _T('9')) {
        (*dp)++;
        break;
      } else {
        *dp = '0';
      }
    }
    if(dp < first) {
      str.insert(0, '1'); // carry propagated all the way the the start
      return 1;
    }
  }
  return 0;
}

static void removeTralingZeroDigits(String &str) {
  while(str.last() == '0') {
    str.removeLast();
  }
  if(str.last() == '.') {
    str.removeLast();
  }
}

// ignore sign
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
    decimals = comma + 1;
    *comma = 0;
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
    commaPos += round5DigitString(ciphers, lastResultDigitPos);
    result += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += substr(ciphers, commaPos, precision);
      }
      if(removeTrailingZeroes) removeTralingZeroDigits(result);
    }
  } else if(commaPos > (intptr_t)ciphers.length()) {
    const intptr_t zeroCount = commaPos - (intptr_t)ciphers.length();
    result += ciphers;
    addZeroes(result, zeroCount);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(!removeTrailingZeroes && (precision > 0)) {
        addZeroes(result, precision);
      }
    }
  } else { // 0 < commaPos <= ciphers.length()
    commaPos += round5DigitString(ciphers, lastResultDigitPos);
    result += substr(ciphers, 0, commaPos);
    if((flags & ios::showpoint) || (precision > 0)) {
      addDecimalPoint(result);
      if(precision > 0) {
        result += substr(ciphers, commaPos, (intptr_t)precision);
        if(removeTrailingZeroes) {
          removeTralingZeroDigits(result);
        } else {
          const intptr_t zeroCount = precision - ((intptr_t)ciphers.length() - commaPos);
          if(zeroCount > 0) {
            addZeroes(result, zeroCount);
          }
        }
      }
    }
  }
}

// ignore sign
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
    exponent = _ttoi(estr+1);
    *estr = 0;
  }

  if(comma != NULL) {
    decimals = comma + 1;
    *comma = 0;
    if(precision+1 < (intptr_t)decimals.length()) {
      decimals = substr(decimals,0,precision+1);
      if(decimals[precision] >= _T('5')) {
        decimals[precision] = '0';
        intptr_t i;
        for(i = (intptr_t)precision-1; i >= 0; i--) {
          TCHAR &digit = decimals[i];
          if(digit < _T('9')) {
            digit++;
            break;
          } else {
            digit = '0';
          }
        }
        if(i < 0) {
          if(mantissa[0] < _T('9')) {
            mantissa[0]++;
          } else {
            mantissa[0] = _T('1');
            exponent++;
          }
        }
      }
    }
  }

  result += mantissa;

  if(removeTrailingZeroes && (precision < (intptr_t)decimals.length())) {
    for(; precision > 0 && decimals[(intptr_t)precision-1] == '0'; precision--) { // remove trailing zeroes from decimals
      decimals.remove(precision-1);
    }
  }

  if((flags & ios::showpoint) || (precision > 0)) {
    addDecimalPoint(result);
    if(precision > 0) {
      result += substr(decimals,0,precision);
    }
  }

  addExponentChar(result);
  result += format(_T("%+03d"), exponent);
}

StrStream &operator<<(StrStream &stream, const Double80 &x) {
  intptr_t          precision  = (intptr_t)stream.getPrecision();
  const FormatFlags flags      = stream.getFlags();

  if(precision < 0) {
    precision = MAXPRECISION;
  }

  String result;
  if(!isfinite(x)) {
    result = StrStream::formatUndefined(x);
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
      if((flags & ios::floatfield) == ios::fixed) {             // Use fixed format
        formatFixed(result, x, precision, flags, expo10, false);
      } else if((flags & ios::floatfield) == ios::scientific) { // Use scientific format
        formatScientific(result, x, precision, flags, false);
      } else if((expo10 < -4) || (expo10 > 14) || (expo10 > 0 && expo10 >= precision) || (expo10 > precision)) { // neither scientific nor fixed format is specified
        precision = max(0,precision-1);
        formatScientific(result, x, precision, flags, (flags & ios::showpoint) == 0);
      } else {
        const intptr_t prec = (precision == 0) ? abs(expo10) : max(0,precision-expo10-1);
        formatFixed(result, x, prec, flags, expo10, ((flags & ios::showpoint) == 0) || precision <= 1);
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
  if (out.flags() & ios::unitbuf) {
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
