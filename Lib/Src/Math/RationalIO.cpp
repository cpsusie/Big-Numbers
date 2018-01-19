#include "pch.h"
#include <Math/Rational.h>

using namespace std;

String toString(const Rational &r, int precision, int width, int flags) {
  StrStream result(precision, width,flags);
  result << r;
  return result;
}

StrStream &operator<<(StrStream &stream, const Rational &r) {
  const long flags = stream.getFlags();
  const int  radix = StreamParameters::getRadix(flags);
  TCHAR tmp[150];
  String result = _rattot(tmp,r, radix);
  if(!isNan(r) && (radix==10) && (flags & ios::showpos) && !r.isNegative()) {
    result = _T("+") + result;
  }

  streamsize fillerLength = stream.getWidth() - (int)result.length();
  if(fillerLength <= 0) {
    stream.append(result);
  } else if(flags & ios::left) {
    stream.append(result).append(spaceString(fillerLength));
  } else {// right align
    stream.append(spaceString(fillerLength)).append(result);
  }
  return stream;
}

template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Rational &r) {
  if(in.ipfx(0)) {
    INT64 numerator, denominator;
    int radix1 = StreamParameters::getRadix(in.flags());
    in >> numerator;
    if(!in.bad()) {
      CharType ch = in.peek();
      if(ch == EOF) in >> ch;
      if(ch == '/') {
        in >> ch;
        int radix2 = StreamParameters::getRadix(in.flags());
        in >> denominator;
      } else {
        denominator = 1;
      }
    }
    if(!in.bad()) {
      try {
        r = Rational(numerator, denominator);
      } catch(...) {
        in.clear(ios::failbit);
        in.isfx();
        throw;
      }
    }
    in.isfx();
  }
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Rational &r) {
  if(out.opfx()) {
    StrStream stream(out);
    stream << r;
    out << (String&)stream;
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
    out.osfx();
  }
  return out;
}

istream &operator>>(istream &in, Rational &r) {
  return ::operator>> <istream,char>(in, r);
}

ostream &operator<<(ostream &out, const Rational &r) {
  return operator<< <ostream>(out, r);
}

std::wistream &operator>>(std::wistream &in, Rational &r) {
  return ::operator>> <std::wistream,wchar_t>(in, r);
}

std::wostream &operator<<(std::wostream &out, const Rational &r) {
  return operator<< <std::wostream>(out, r);
}
