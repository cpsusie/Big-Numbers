#include "pch.h"
#include <StrStream.h>
#include <Math/Complex.h>

using namespace std;

StrStream &operator<<(StrStream &stream, const Complex &c) {
  if(c.im == 0) {
    stream << c.re;
  } else {
    stream.append(_T("("));
    stream << c.re;
    stream.append(_T(","));
    stream << c.im;
    stream.append(_T(")"));
  }
  return stream;
}

/* inputformat for Complex;
    re
    (re)
    (re,im)
*/
template <class IStreamType, class CharType> IStreamType &getComplex(IStreamType &in, Complex &c) {
  if(in.flags() & ios::skipws) skipspace(in);
  CharType ch = in.peek();
  Real re, im;
  if(ch != '(') {
    in >> re;
    im = 0;
  } else {
    in.get();
    skipspace(in);
    in >> re;
    if(!in) goto Fail;
    skipspace(in);
    ch = in.peek();
    if(ch == ',')  {
      in.get();
      skipspace(in);
      in >> im;
      if(!in) goto Fail;
      skipspace(in);
      ch = in.peek();
    }
    if(ch == ')') {
      in.get();
    } else {
      goto Fail;
    }
  }
  if(in) {
    c = Complex(re,im);
    return in;
  }
Fail:
  in.setstate(ios::failbit);
  return in;
}

template <class OStreamType> OStreamType &putComplex(OStreamType &out, const Complex &c) {
  StrStream stream(out);
  stream << c;
  out << (String&)stream;
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

istream &operator>>(istream &in, Complex &c) {
  return getComplex<istream, char>(in, c);
}

ostream &operator<<(ostream &out, const Complex &c) {
  return putComplex(out, c);
}

wistream &operator>>(wistream &in, Complex &c) {
  return getComplex<wistream, wchar_t>(in, c);
}

wostream &operator<<(wostream &out, const Complex &c) {
  return putComplex(out, c);
}

String toString(const Complex &c, StreamSize precision, StreamSize width, FormatFlags flags) {
  tostrstream stream;
  stream.width(width);
  stream.precision(precision);
  stream.flags(flags);
  stream << c;
  return stream.str().c_str();
}
