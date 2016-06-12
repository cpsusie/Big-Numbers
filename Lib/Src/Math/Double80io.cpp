#include "pch.h"
#include <ctype.h>
#include <Math/Double80.h>

using namespace std;

static const TCHAR *findFirstDigit(const String &str) {
  for(const TCHAR *cp = str.cstr(); *cp; cp++) {
    if(_istdigit(*cp)) {
      return cp;
    }
  }
  return NULL;
}

#define MAXPRECISION 18

static void formatNan(String &result, const Double80 &x) {
  if(!isInfinity(x)) {
    result = _T("Nan");
  } else if(isPInfinity(x)) {
    result = _T("+Infinity");
  } else if(isNInfinity(x)) {
    result = _T("-Infinity");
  }
}

#define addDecimalPoint(s) { s += _T("."); }
#define addExponentChar(s) { s += ((flags & ios::uppercase) ? _T("E") : _T("e")); }

static void formatFixed(String &result, const Double80 &x, int precision, long flags, bool removeTrailingZeroes) {
  String        str       = round(x,precision).toString();
  const TCHAR  *mantissa  = findFirstDigit(str);
  TCHAR        *comma     = _tcschr(str.cstr(),_T('.'));
  TCHAR        *decimals  = NULL;
  TCHAR        *estr      = _tcschr(str.cstr(),_T('e'));
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
      result += ciphers + spaceString(exponent - (int)ciphers.length() + 1,_T('0'));
      if(flags & ios::showpoint || precision > 0) {
        addDecimalPoint(result);
      }
      if((precision > 0) && (flags & ios::fixed)) {
        result += spaceString(precision,_T('0'));
      }
    } else { // 0 <= exponent < ciphers.length()
      const int dotPosition = exponent+1;
      result += substr(ciphers,0,dotPosition);

      String decimals = substr(ciphers,dotPosition,precision);
      if(precision > 0) {
        if(removeTrailingZeroes) {
          for(int i = decimals.length()-1; i >= 0 && decimals[i] == _T('0');) { // remove trailing zeroes from decimals
            decimals.remove(i--);
          }
        } else { // add zeroes to specified precisionision
          decimals += spaceString(precision - (ciphers.length() - dotPosition),_T('0'));
        }
      }

      if((flags & ios::showpoint) || decimals.length() > 0) {
        addDecimalPoint(result);
        result += decimals;
      }
    }
  } else { // exponent < 0
    int leadingZeroes = -1 - exponent;
    leadingZeroes = min(leadingZeroes,precision);
    result += _T("0");
    addDecimalPoint(result);
    result += spaceString(leadingZeroes,_T('0'));
    result += substr(ciphers, 0, precision - leadingZeroes);
    if(removeTrailingZeroes) {
      for(int i = result.length()-1; i >= 0 && result[i] == _T('0');) { // remove trailing zeroes from result
        result.remove(i--);
      }
    } else {
      result += spaceString(precision - leadingZeroes - ciphers.length(),_T('0'));
    }
  }
}

static void formatScientific(String &result, const Double80 &x, int precision, long flags, int expo10, bool removeTrailingZeroes) {
  bool exponentCharAdded = false;

  String  str = x.toString();
  String  mantissa  = substr(findFirstDigit(str),0,1);
  String  decimals;
  TCHAR   *comma    = _tcschr(str.cstr(),_T('.'));
  TCHAR   *estr     = _tcschr(str.cstr(),_T('e'));
  int     exponent  = 0;

  if(estr != NULL) {
    exponent = _ttoi(estr+1);
    *estr = 0;
  }

  if(comma != NULL) {
    decimals = comma + 1;
    *comma = 0;
    if(precision+1 < (int)decimals.length()) {
      decimals = substr(decimals,0,precision+1);
      if(decimals[precision] >= _T('5')) {
        decimals[precision] = _T('0');
        int i;
        for(i = precision-1; i >= 0; i--) {
          TCHAR &digit = decimals[i];
          if(digit < _T('9')) {
            digit++;
            break;
          } else {
            digit = _T('0');
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

  if(removeTrailingZeroes && precision < (int)decimals.length()) {
    for(; precision > 0 && decimals[precision-1] == _T('0'); precision--) { // remove trailing zeroes from decimals
      decimals.remove(precision-1);
    }
  }

  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(result);
    if(precision > 0) {
      result += substr(decimals,0,precision);
    }
  }

  addExponentChar(result);
  result += format(_T("%+03d"),exponent);
}

StrStream &operator<<(StrStream &stream, const Double80 &x) {
  int  precision  = stream.getPrecision();
  long flags = stream.getFlags();

  if(precision < 0) {
    precision = MAXPRECISION;
  }

  String result;
  if(isNan(x)) {
    formatNan(result,x);
  } else { // x defined
    if(x.isNegative()) {
      result = _T("-");
    } else if(flags & ios::showpos) {
      result = _T("+");
    }

    if(x == Double80::zero) {
      StrStream::formatZero(result,precision,flags,MAXPRECISION);
    } else { // x defined && x != 0
      if((flags & (ios::scientific|ios::fixed)) == ios::fixed) { // Use fixed format
        formatFixed(result, x, precision, flags, false);
      } else {
        int expo10 = Double80::getExpo10(x);
        if((flags & (ios::scientific|ios::fixed)) == ios::scientific) { // Use scientific format
          formatScientific(result, x, precision, flags, expo10, false);
        } else {
          if(expo10 < -4 || expo10 > 14 || (expo10 > 0 && expo10 >= precision) || expo10 > precision) { // neither scientific nor fixed format is specified
            precision = max(0,precision-1);
            formatScientific(result, x, precision, flags, expo10, (flags & ios::showpoint) == 0);
          } else {
            const int prec = (precision == 0) ? abs(expo10) : max(0,precision-expo10-1);
            formatFixed(result, x, prec, flags, ((flags & ios::showpoint) == 0) || precision <= 1);
          }
        }
      }
    } // x defined && x != 0
  } // end x defined

  const int fillerLength = stream.getWidth() - result.length();
  if(fillerLength <= 0) {
    stream.append(result);
  } else if((flags & (ios::left|ios::right)) == ios::left) { // adjust left iff only ios::left is set
    stream.append(result).append(spaceString(fillerLength));
  } else {// right align
    stream.append(spaceString(fillerLength)).append(result);
  }
  return stream;
}

String toString(const Double80 &x, int precision, int width, int flags) {
  StrStream stream(precision,width,flags);
  stream << x;
  return (String)stream;
}

#define peekChar(in,ch)          { ch = in.peek(); if(ch == EOF) in >> ch; }
#define appendCharGetNext(in,ch) { in >> ch; buf += ch; peekChar(in,ch);   }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
    peekChar(in, ch);
    if(!_istspace(ch)) {
      return;
    }
  }
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Double80 &x) {
  if(in.ipfx(0)) {
    String   buf;
    CharType ch;
    bool     gotDigits = false;
  
    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);
    if((ch == _T('-')) || (ch == _T('+'))) {
      appendCharGetNext(in, ch);
    }
    while(_istdigit(ch)) {
      appendCharGetNext(in, ch);
      gotDigits = true;
    }
    if(ch == _T('.')) {
      appendCharGetNext(in, ch);
      while(_istdigit(ch)) {
        appendCharGetNext(in, ch);
        gotDigits = true;
      }
    }
    if(!gotDigits) {
      in.putback(ch);
      in.clear(ios::failbit);
      in.isfx();
      return in;
    }
    if((ch == _T('e')) || (ch == _T('E'))) {
      appendCharGetNext(in, ch);
      if((ch == _T('-')) || (ch == _T('+'))) {
        appendCharGetNext(in, ch);
      }
      while(_istdigit(ch)) {
        appendCharGetNext(in, ch);
      }
    }
    try {
      x = Double80(buf.cstr());
    } catch(...) {
      in.clear(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Double80 &x) {
  if(out.opfx()) {
    StrStream stream(out);
    stream << x;
    out << stream.cstr();
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
    out.osfx();
  }
  return out;
}

tistream &operator>>(tistream &in, Double80 &x) {
  return ::operator>> <tistream,TCHAR>(in, x);
}

tostream &operator<<(tostream &out, const Double80 &x) {
  return ::operator<< <tostream> (out, x);
}
