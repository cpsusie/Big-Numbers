#pragma once
#include "RationalStr.h"

namespace RationalIO {

using namespace std;
using namespace RationalStr;

template<typename IStreamType> IStreamType &getRational(IStreamType &in, Rational &r) {
  const FormatFlags flags = in.flags();
  const int         base = StreamParameters::radix(flags);
  INT64             num, den;
  if(base == 10) {
    in >> num;
    if(in) {
      in.flags(flags & ~ios::skipws);
      if(in.peek() != '/') {
        den = 1;
      } else {
        in.get();
        in >> den;
      }
      in.flags(flags);
    }
  } else {
    UINT64 unum, uden;
    in >> unum;
    if(in) {
      in.flags(flags & ~ios::skipws);
      if(in.peek() != '/') {
        uden = 1;
      } else {
        in.get();
        in >> uden;
      }
      in.flags(flags);
      if(in) {
        num = unum;
        den = uden;
      }
    }
  }
  if(in) {
    r = Rational(num, den);
  }
  return in;
}

template<typename OStreamType> OStreamType &putRational(OStreamType &out, const Rational &r) {
  if(r.isInteger()) {
    out << r.getNumerator();
  } else {
    String buf;
    out << formatRational(buf, r, StreamParameters(out));
    if(out.flags() & ios::unitbuf) {
      out.flush();
    }
  }
  return out;
}

}; // namespace RationalIO
