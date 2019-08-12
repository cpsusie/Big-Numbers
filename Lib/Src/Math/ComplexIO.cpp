#include "pch.h"
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

#include "IStreamUtil.h"

/* inputformat for Complex;
    re
    (re)
    (re,im)
*/
template <class IStreamType, class CharType> IStreamType &operator>>(IStreamType &in, Complex &c) {
  SETUPISTREAM(in)
  Real re, im;
  if(ch != '(') {
    in >> re;
    im = 0;
  } else {
    nextchar(in);
    skipSpace(in);
    in >> re;
    if(!in) goto Fail;
    skipLeadingspace(in);
    if(ch == ',')  {
      nextchar(in);
      skipSpace(in);
      in >> im;
      if(!in) goto Fail;
      skipLeadingspace(in);
    }
    if(ch == ')') {
      nextchar(in);
    } else {
      goto Fail;
    }
  }
  if(in) {
    c = Complex(re,im);
    RESTOREISTREAM(in);
    return in;
  }
Fail:
  ungetbuf(in);
  RESTOREISTREAM(in);
  in.setstate(ios::failbit);
  return in;
}

template <class OStreamType> OStreamType &operator<<(OStreamType &out, const Complex &c) {
  StrStream stream(out);
  stream << c;
  out << stream.cstr();
  return out;
}

istream &operator>>(istream &in, Complex &c) {
  return ::operator>> <istream, char>(in, c);
}

ostream &operator<<(ostream &out, const Complex &c) {
  return ::operator<< <ostream>(out, c);
}

wistream &operator>>(wistream &in, Complex &c) {
  return ::operator>> <wistream, wchar_t>(in, c);
}

wostream &operator<<(wostream &out, const Complex &c) {
  return ::operator<< <wostream>(out, c);
}
