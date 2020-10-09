#pragma once

#include <Math/Complex.h>
#include <StrStream.h>

namespace ComplexIO {

using namespace std;
using namespace IStreamHelper;

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
    if(ch == ',') {
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
    c = Complex(re, im);
    return in;
  }
Fail:
  in.setstate(ios::failbit);
  return in;
}

template<typename OStreamType> OStreamType &putComplex(OStreamType &out, const Complex &c) {
  if(c.im == 0) {
    out << c.re;
  } else {
    StreamParameters param(out);
    param.width(0);
    ostringstream tmp;
    tmp << param << "(" << c.re << "," << c.im << ")";
    out << tmp.str().c_str();
  }
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

}; // namespace ComplexIO
