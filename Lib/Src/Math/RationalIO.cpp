#include "pch.h"
#include <StrStream.h>
#include <Math/Rational.h>

using namespace std;
using namespace OStreamHelper;

// assume !r.isInteger()
// Return dst
static String &formatRational(String &dst, const Rational &r, StreamParameters &param) {
  const int   base     = param.radix();
  FormatFlags flags    = param.flags();
  bool        negative = r.isNegative();
  String      buf;
  if(!isfinite(r)) {
    TCHAR tmp[150];
    buf    = formatUndefined(tmp, _fpclass(r), (flags & ios::uppercase), true);
    flags &= ~(ios::hexfloat | ios::basefield | ios::showbase);
  } else {
    StreamParameters numParam(param), denParam(param);
    numParam.width(0);
    numParam.flags(flags & ~(ios::showpos | ios::showbase));
    denParam.width(0);
    denParam.flags(flags & ~ios::showpos);
    if(base != 10) {
      negative = false;
      flags   &= ~ios::showpos;
    }
    TowstringStream numStr(numParam), denStr(denParam);
    numStr << ((negative && (base == 10)) ? -r.getNumerator() : r.getNumerator());
    denStr << r.getDenominator(); // denominator always positive
    buf = numStr.str().c_str();
    buf += '/';
    buf += denStr.str().c_str();
    if((base==16) && (flags & ios::uppercase)) {
      buf = toUpperCase(buf);
    }
  }
  param.flags(flags);
  return formatFilledNumericField(dst, buf, negative, param);
}

template <typename IStreamType> IStreamType &getRational(IStreamType &in, Rational &r) {
  const FormatFlags flags = in.flags();
  const int         base  = StreamParameters::radix(flags);
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

template <typename OStreamType> OStreamType &putRational(OStreamType &out, const Rational &r) {
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

istream &operator>>(istream &in, Rational &r) {
  return getRational(in, r);
}

ostream &operator<<(ostream &out, const Rational &r) {
  return putRational(out, r);
}

wistream &operator>>(wistream &in, Rational &r) {
  return getRational(in, r);
}

wostream &operator<<(wostream &out, const Rational &r) {
  return putRational(out, r);
}
