#include "pch.h"
#include <StrStream.h>
#include <Math/Rational.h>

using namespace std;

class RationalStrStream : public StrStream {
public:
  RationalStrStream(const ostream &s) : StrStream(s) {
  }
  RationalStrStream(const wostream &s) : StrStream(s) {
  }
  RationalStrStream &operator<<(const Rational &r);
};

// assume !r.isInteger()
RationalStrStream &RationalStrStream::operator<<(const Rational &r) {
  const int   base     = radix();
  FormatFlags flg      = flags();
  bool        negative = r.isNegative();
  String      buf;
  if(!isfinite(r)) {
    TCHAR tmp[150];
    buf = formatUndefined(tmp, _fpclass(r), (flg & ios::uppercase), true);
    flg &= ~(ios::hexfloat | ios::basefield | ios::showbase);
  } else {
    StrStream numStr(*this), denStr(*this);
    numStr.width(0); denStr.width(0);
    numStr.flags(flg & ~(ios::showpos | ios::showbase));
    denStr.flags(flg & ~ios::showpos);
    numStr << ((negative && (base == 10)) ? -r.getNumerator() : r.getNumerator());
    if(base != 10) {
      negative = false;
      flg &= ~ios::showpos;
    }
    denStr << r.getDenominator(); // denominator always positive
    buf = numStr;
    buf += _T('/');
    buf += denStr;
    if((base==16) && (flg & ios::uppercase)) {
      buf = toUpperCase(buf);
    }
  }
  formatFilledNumericField(buf, negative, flg);
  return *this;
}

template <typename IStreamType> IStreamType &getRational(IStreamType &in, Rational &r) {
  const FormatFlags flg  = in.flags();
  const int         base = StreamParameters::radix(flg);
  INT64             num, den;
  if(base == 10) {
    in >> num;
    if(in) {
      in.flags(flg & ~ios::skipws);
      if(in.peek() != '/') {
        den = 1;
      } else {
        in.get();
        in >> den;
      }
      in.flags(flg);
    }
  } else {
    UINT64 unum, uden;
    in >> unum;
    if(in) {
      in.flags(flg & ~ios::skipws);
      if(in.peek() != '/') {
        uden = 1;
      } else {
        in.get();
        in >> uden;
      }
      in.flags(flg);
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
    RationalStrStream stream(out);
    stream << r;
    out << (String&)stream;
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

String toString(const Rational &r, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TowstringStream(precision, width, flags) << r).str().c_str();
}
