#include "pch.h"
#include <String.h>
#include <ctype.h>

using namespace std;

TCHAR BigRealStream ::setSpaceChar(TCHAR value) { 
  TCHAR ospac = m_spaceChar; 
  m_spaceChar = value; 
  return ospac;
}

#define addDecimalPoint(s) { if(!decimalPointAdded) { s += "."; decimalPointAdded = true; } }
#define addExponentChar(s) { s += ((flags & ios::uppercase) ? _T("E") : _T("e")); }

void BigReal::formatFixed(String &result, int precision, long flags, bool removeTrailingZeroes) const {
  bool decimalPointAdded = false;

  const BigReal nn(round(*this,precision));
  if(nn.isZero()) {
    StrStream::formatZero(result,precision,flags);
    return;
  }

  const Digit *digit = nn.m_first;
  int d              = getExpo10(nn);
  int decimalsDone   = 0;

  if(d < 0) { // first handle integerpart
    result += _T("0");
  } else {
    result += format(_T("%lu"), digit->n);
    d -= BigReal::getDecimalDigitCount(digit->n);
    for(digit = digit->next; digit != NULL && (d >= 0); d -= LOG10_BIGREALBASE, digit = digit->next) {
      result += format(_T("%0*.*lu"), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n);
    }
    result += spaceString(d+1,'0');
    d = -1;
  }

  // now handle fraction if wanted
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(result);

    if(precision > 0) {
      for(int i = 0; (i - LOG10_BIGREALBASE > d) && (decimalsDone < precision); i -= LOG10_BIGREALBASE) {
        int partLength = LOG10_BIGREALBASE;
        int rest       = precision - decimalsDone;
        if(rest < LOG10_BIGREALBASE) {
          partLength = rest;
        }
        result += format(_T("%0*.*lu"), partLength,partLength,0);
        decimalsDone += partLength;
      }
      for(;digit && (decimalsDone < precision); digit = digit->next) {
        int part       = digit->n;
        int partLength = LOG10_BIGREALBASE;
        int rest       = precision - decimalsDone;
        if(rest < LOG10_BIGREALBASE) {
          partLength = rest;
          part /= BigReal::pow10(LOG10_BIGREALBASE - rest);
        }
        result += format(_T("%0*.*lu"), partLength,partLength,part);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && decimalsDone < precision) {
        result += spaceString(precision-decimalsDone,'0');
      }
      if(removeTrailingZeroes) {
        intptr_t i;
        for(i = result.length()-1; i >= 0 && result[i]=='0';) {
          result.remove(i--);
        }
        if(!(flags & ios::showpoint) && result[i] == _T('.')) {
          result.remove(i--);
        }
      }
    }
  }
}

void BigReal::formatScientific(String &result, int precision, long flags, int expo10, bool removeTrailingZeroes) const {
  bool decimalPointAdded = false;
  bool exponentCharAdded = false;

  const BigReal nn(round(*this,precision - expo10));
  const Digit *digit = nn.m_first;
  int decimalsDone   = 0;
  int scale          = getDecimalDigitCount(digit->n) - 1;
  int scaleE10       = pow10(scale);

  result += format(_T("%d"), digit->n / scaleE10);
  if((flags & ios::showpoint) || precision > 0) {
    addDecimalPoint(result);

    if(precision > 0) {
      int fraction = digit->n % scaleE10;
      if(precision < scale) {
        fraction /= pow10(scale-precision);
        decimalsDone = precision;
      } else {
        decimalsDone = scale;
      }
      if(decimalsDone > 0) {
        result += format(_T("%0*.*d"), decimalsDone,decimalsDone,fraction);
      }
      for(digit = digit->next; digit != NULL && decimalsDone < precision; digit = digit->next) { // now handle tail
        int part       = digit->n;
        int partLength = LOG10_BIGREALBASE;
        int rest       = precision - decimalsDone;
        if(rest < LOG10_BIGREALBASE) {
          partLength = rest;
          part /= pow10(LOG10_BIGREALBASE - rest);
        }
        result += format(_T("%0*.*lu"), partLength,partLength,part);
        decimalsDone += partLength;
      }
      if(!removeTrailingZeroes && decimalsDone < precision) {
        result += spaceString(precision-decimalsDone,'0');
      }
      if(removeTrailingZeroes) {
        intptr_t i;
        for(i = result.length()-1; i >= 0 && result[i]=='0';) {
          result.remove(i--);
        }
        if(!(flags & ios::showpoint) && result[i] == _T('.')) {
          result.remove(i--);
        }
      }
    }
  }
  addExponentChar(result);
  result += format(_T("%+03d"), BigReal::getExpo10(nn));
}

void BigReal::formatWithSpaceChar(String &result, TCHAR spaceChar) const {
  if(isZero()) {
    result += _T("0");
  } else {
    bool decimalPointAdded = false;
    int d = m_expo;
    if(d < 0) {
      result += _T("0");
      addDecimalPoint(result);
      for(int i = -1; i > d; i--) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE,LOG10_BIGREALBASE,0,spaceChar);
      }
      for(const Digit *digit = m_first; digit; digit = digit->next) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n,spaceChar);
      }
    } else {
      result += format(_T("%lu%c"), m_first->n,spaceChar);
      if(d-- == 0 && m_first->next) {
        addDecimalPoint(result);
      }
      for(Digit *digit = m_first->next; digit; digit = digit->next) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n,spaceChar);
        if(d-- == 0 && digit->next) 
          addDecimalPoint(result);
      }
      for(;d >= 0;d--) {
        result += format(_T("%0*.*lu%c"), LOG10_BIGREALBASE,LOG10_BIGREALBASE,0,spaceChar);
      }
    }
    if(result[result.length()-1] == spaceChar) {
      result.remove(result.length()-1);
    }
  }
}

BigRealStream &operator<<(BigRealStream &stream, const BigReal &x) {

  ENTER_CRITICAL_SECTION_BIGREAL_DEBUGSTRING()

  try {
    int   precision = stream.getPrecision();
    long  flags     = stream.getFlags();
    TCHAR spaceChar = stream.getSpaceChar();

    String result;
    if(x.isNegative()) {
      result += _T("-");
    } else if(flags & ios::showpos) {
      result += _T("+");
    }

    if(spaceChar != 0) {
      x.formatWithSpaceChar(result,spaceChar);
    } else {
      if(x.isZero()) {
        StrStream::formatZero(result, precision, flags);
      } else { // x defined && x != 0
        if((flags & (ios::scientific|ios::fixed)) == ios::fixed) { // Use fixed format
          x.formatFixed(result, precision, flags, false);
        } else {
          int expo10 = BigReal::getExpo10(x);
          if((flags & (ios::scientific|ios::fixed)) == ios::scientific) { // Use scientific format
            x.formatScientific(result, precision, flags, expo10, false);
          } else {
            if(expo10 < -4 || expo10 > 14 || (expo10 > 0 && expo10 >= precision) || expo10 > precision) { // neither scientific nor fixed format (or both) are specified
              precision = max(0,precision-1);
              x.formatScientific(result, precision, flags, expo10, (flags & ios::showpoint) == 0);
            } else {
              const int prec = (precision == 0) ? abs(expo10) : max(0,precision-expo10-1);
              x.formatFixed(result, prec, flags, ((flags & ios::showpoint) == 0) || precision <= 1);
            }
          }
        }
      } // x defined && x != 0
    }

    const int fillerLength = stream.getWidth() - (int)result.length();
    if(fillerLength <= 0) {
      stream.append(result);
    } else if ((flags & (ios::left | ios::right)) == ios::left) { // adjust left iff only ios::left is set
      stream.append(result).append(spaceString(fillerLength));
    } else { // right align
      stream.append(spaceString(fillerLength)).append(result);
    }
  } catch(Exception e) {
    LEAVE_CRITICAL_SECTION_BIGREAL_DEBUGSTRING(;);
    throw e;
  }

  LEAVE_CRITICAL_SECTION_BIGREAL_DEBUGSTRING(;);

  return stream;
}

BigRealStream &operator<<(BigRealStream &out, const FullFormatBigReal &n) {
  int e = BigReal::getExpo10(n);
  int precision;
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
    for(unsigned int last = n.getLastDigit(); last % 10 == 0; last /= 10) precision--;
  }
  out.setFlags((out.getFlags() | ios::scientific) & ~ios::fixed);
  out.setPrecision(precision);
  out << (BigReal)n;
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
  out << (BigReal)n;
  return out;
}

#define peekChar(in,ch)           { ch = in.peek(); if(ch == EOF) in >> ch; }
#define appendCharGetNext(in, ch) { in >> ch; buf += ch; peekChar(in,ch);   }

template <class IStreamType, class CharType> void eatWhite(IStreamType &in) {
  CharType ch;
  for(;;in >> ch) {
	  peekChar(in, ch);
	  if(!_istspace(ch)) {
	    return;
	  }
  }
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, BigReal &x) {
  if(in.ipfx(0)) {
    String   buf;
    CharType ch;
    bool     gotDigits = false;

    eatWhite<IStreamType,CharType>(in);
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
      in.setf(ios::failbit);
      in.isfx();
      return in;
    }
    if(ch == 'e' || ch == 'E') {
      appendCharGetNext(in,ch);
      if((ch == _T('-')) || (ch == _T('+'))) {
        appendCharGetNext(in,ch);
      }
      while(_istdigit(ch)) {
        appendCharGetNext(in,ch);
      }
    }
    try {
      x = BigReal(buf.cstr(),x.getDigitPool());
    } catch(...) {
      in.setf(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class IStreamType, class CharType> IStreamType &operator >> (IStreamType &in, BigInt &n) {
  if(in.ipfx(0)) {
    String buf;
    TCHAR  ch;
    bool   gotDigits = false;

    eatWhite<IStreamType, CharType>(in);
    peekChar(in, ch);
    if((ch == _T('-')) || (ch == _T('+'))) {
      appendCharGetNext(in, ch);
    }
    while(_istdigit(ch)) {
      appendCharGetNext(in, ch);
      gotDigits = true;
    }
    if(!gotDigits) {
      in.putback(ch);
      in.setf(ios::failbit);
      in.isfx();
      return in;
    }
    try {
      n = BigInt(buf.cstr(), n.getDigitPool());
    } catch(...) {
      in.setf(ios::failbit);
      in.isfx();
      throw;
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const BigReal &x) {
  if(out.opfx()) {
    BigRealStream buf(out);
    buf << x;
    out << buf.cstr();
    out.osfx();
  }
  return out;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const BigInt &n) {
  if(out.opfx()) {
    BigRealStream buf(out);
    buf << n;
    out << buf.cstr();
    out.osfx();
  }
  return out;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const FullFormatBigReal &x) {
  if(out.opfx()) {
    BigRealStream buf(out);
    buf << x;
    out << buf.cstr();
    out.osfx();
  }
  return out;
}

tistream &operator>>(tistream &in, BigReal &x) {
  return ::operator>><tistream,TCHAR>(in, x);
}

tistream &operator>>(tistream &in, BigInt &n) {
  return ::operator>><tistream, TCHAR>(in, n);
}

tostream &operator<<(tostream &out, const BigReal &x) {
  return ::operator<< <tostream>(out, x);
}

tostream &operator<<(tostream &out, const BigInt &n) {
  return ::operator<< <tostream>(out, n);
}

tostream &operator<<(tostream &out, const FullFormatBigReal &x) {
	return ::operator<< <tostream>(out, x);
}

void BigReal::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void FullFormatBigReal::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void BigInt::print(FILE *f, bool spacing) const {
  BigRealStream buf;
  if(spacing) {
    buf.setSpaceChar(_T(':'));
  }
  buf << *this;
  _ftprintf(f,_T("%s"), buf.cstr());
}

void BigReal::dump(FILE *f) const {
  _ftprintf(f, _T("["));
  if(isNegative()) {
    _ftprintf(f, _T("-"));
  }
  int d = m_expo;
  for(const Digit *digit = m_first; digit; digit = digit->next) {
    _ftprintf(f,_T("%0*.*lu "), LOG10_BIGREALBASE,LOG10_BIGREALBASE,digit->n);
  }
  _ftprintf(f,_T(" [expo,low,length]:[%4d,%4d,%4d]]"), m_expo,m_low,getLength());
}

String BigReal::toString() const {
  return ::toString(*this);
}

String toString(const BigReal &n, int precision, int width, int flags) {
  BigRealStream buf(precision,width,flags);
  buf << n;
  return buf;
}
