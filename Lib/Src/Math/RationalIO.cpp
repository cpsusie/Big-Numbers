#include "pch.h"
#include <Math/Rational.h>

using namespace std;

String toString(const Rational &r, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (StrStream(precision, width, flags) << r).toString();
}

StrStream &operator<<(StrStream &stream, const Rational &r) {
  const FormatFlags flags = stream.getFlags();
  const int         radix = StreamParameters::getRadix(flags);
  TCHAR tmp[150];
  String result = _rattot(tmp,r, radix);
  if(isfinite(r) && (radix==10) && (flags & ios::showpos) && !r.isNegative()) {
    result = _T("+") + result;
  }

  const intptr_t fillerLength = (intptr_t)stream.getWidth() - (intptr_t)result.length();
  if(fillerLength <= 0) {
    stream.append(result);
  } else if(flags & ios::left) {
    stream.append(result).append(spaceString(fillerLength));
  } else {// right align
    stream.append(spaceString(fillerLength)).append(result);
  }
  return stream;
}

#include "IStreamUtil.h"

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Rational &r) {
  SETUPISTREAM(in)
  INT64 numerator, denominator;
  int radix1 = StreamParameters::getRadix(in.flags());
  in >> numerator;
  if(in) {
    if(in.peek() == '/') {
      in >> ch;
      int radix2 = StreamParameters::getRadix(in.flags());
      in >> denominator;
    } else {
      denominator = 1;
    }
  }
  if(in) {
    try {
      r = Rational(numerator, denominator);
    } catch(...) {
      ungetbuf(in);
      RESTOREISTREAM(in);
      in.setstate(ios::failbit);
      throw;
    }
  }
  RESTOREISTREAM(in);
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Rational &r) {
  StrStream stream(out);
  stream << r;
  out << (String&)stream;
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

istream &operator>>(istream &in, Rational &r) {
  return ::operator>> <istream,char>(in, r);
}

ostream &operator<<(ostream &out, const Rational &r) {
  return operator<< <ostream>(out, r);
}

wistream &operator>>(wistream &in, Rational &r) {
  return ::operator>> <wistream,wchar_t>(in, r);
}

wostream &operator<<(wostream &out, const Rational &r) {
  return operator<< <wostream>(out, r);
}
