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

#define addDecimalPoint(s) { s += _T("."); }
#define addExponentChar(s) { s += ((flags & ios::uppercase) ? _T("E") : _T("e")); }

static void formatFixed(String &result, const Double80 &x, StreamSize precision, FormatFlags flags, int expo10, bool removeTrailingZeroes) {
  TCHAR tmp[50];
  d80tot(tmp, (precision >= MAXPRECISION) ? x : round(x,(int)precision));
  const TCHAR  *mantissa  = findFirstDigit(tmp);
  TCHAR        *comma     = _tcschr(tmp,_T('.'));
  TCHAR        *decimals  = NULL;
  TCHAR        *estr      = _tcschr(tmp,_T('e'));
  int           exponent  = 0;

  if(comma != NULL) {
    decimals = comma + 1;
    *comma = 0;
  }
  if(estr != NULL) {
    *(estr++) = 0;
    exponent = _ttoi(estr);
  }
  String ciphers = mantissa;
  if(decimals != NULL) {
    ciphers += decimals;
  }
  if(exponent >= 0) {
    if(exponent >= (int)ciphers.length()) {
      result += ciphers + spaceString(exponent - (int)ciphers.length() + 1,'0');
      if(flags & ios::showpoint || precision > 0) {
        addDecimalPoint(result);
      }
      if((precision > 0) && (flags & ios::fixed)) {
        result += spaceString(precision,'0');
      }
    } else { // 0 <= exponent < ciphers.length()
      const int dotPosition = exponent+1;
      result += substr(ciphers,0,dotPosition);

      String decimals = substr(ciphers,dotPosition,(intptr_t)precision);
      if(precision > 0) {
        if(removeTrailingZeroes) {
          while((decimals.length()>0) && (decimals.last() == '0')) { // remove trailing zeroes from decimals
            decimals.removeLast();
          }
        } else { // add zeroes to specified precisionision
          decimals += spaceString(precision - ((int)ciphers.length() - dotPosition),'0');
        }
      }

      if((flags & ios::showpoint) || decimals.length() > 0) {
        addDecimalPoint(result);
        result += decimals;
      }
    }
  } else { // exponent < 0
    intptr_t leadingZeroes = -1 - exponent;
    leadingZeroes = min(leadingZeroes,(intptr_t)precision);
    result += _T("0");
    addDecimalPoint(result);
    result += spaceString(leadingZeroes,'0');
    result += substr(ciphers, 0, (intptr_t)precision - leadingZeroes);
    if(removeTrailingZeroes) {
      for(int i = (int)result.length()-1; i >= 0 && result[i] == '0';) { // remove trailing zeroes from result
        result.remove(i--);
      }
    } else {
      result += spaceString(precision - leadingZeroes - (int)ciphers.length(),'0');
    }
  }
}

static void formatScientific(String &result, const Double80 &x, StreamSize precision, FormatFlags flags, bool removeTrailingZeroes) {
  TCHAR tmp[50];
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
      decimals = substr(decimals,0,(intptr_t)precision+1);
      if(decimals[(intptr_t)precision] >= _T('5')) {
        decimals[(intptr_t)precision] = '0';
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
      decimals.remove((intptr_t)precision-1);
    }
  }

  if((flags & ios::showpoint) || (precision > 0)) {
    addDecimalPoint(result);
    if(precision > 0) {
      result += substr(decimals,0,(intptr_t)precision);
    }
  }

  addExponentChar(result);
  result += format(_T("%+03d"), exponent);
}

StrStream &operator<<(StrStream &stream, const Double80 &x) {
  StreamSize        precision  = stream.getPrecision();
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
        const StreamSize prec = (precision == 0) ? abs(expo10) : max(0,precision-expo10-1);
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
