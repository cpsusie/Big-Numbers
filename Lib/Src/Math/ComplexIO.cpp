#include "pch.h"
#include <StrStream.h>
#include <Math/Complex.h>

using namespace std;

/* inputformat for Complex;
    re
    (re)
    (re,im)
*/
template <typename IStreamType, typename CharType> IStreamType &getComplex(IStreamType &in, Complex &c) {
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

template<typename OStreamType, typename SStreamType> OStreamType &putComplex(OStreamType &out, const Complex &c) {
  if(c.im == 0) {
    out << c.re;
  } else {
    StreamParameters param(out);
    param.width(0);
    SStreamType tmp;
    tmp << param << "(" << c.re << "," << c.im << ")";
    out << tmp.str();
  }
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

istream &operator>>(istream &in, Complex &c) {
  return getComplex<istream, char>(in, c);
}

ostream &operator<<(ostream &out, const Complex &c) {
  return putComplex<ostream, stringstream>(out, c);
}

wistream &operator>>(wistream &in, Complex &c) {
  return getComplex<wistream, wchar_t>(in, c);
}

wostream &operator<<(wostream &out, const Complex &c) {
  return putComplex<wostream, wstringstream>(out, c);
}

String toString(const Complex &c, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << c).str().c_str();
}
