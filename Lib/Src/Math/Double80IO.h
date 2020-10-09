#pragma once

#include <Singleton.h>
#include "Double80Str.h"

namespace Double80IO {

using namespace std;
using namespace Double80Str;
using namespace OStreamHelper;
using namespace IStreamHelper;

class D80StringFields: public FloatStringFields {
public:
  D80StringFields(const Double80 &x);
};

#define MAXPRECISION numeric_limits<Double80>::digits10

// See Solution by Stephan T. Lavavej [MSFT] on
// https ://developercommunity.visualstudio.com/content/problem/1145155/wrong-output-in-x64-release-mode-c.html?childToView=1159350#comment-1159350
// For fixed and scientific, precision is the number of digits after the decimal point, ignoring any digits before the decimal point.
// For general(aka defaultfloat in C++), precision is the total number of digits, including those before the decimal point.
// Requesting showpoint is equivalent to the printf flag `#` which says "always print the decimal point, 
// even if there are no decimal digits to the right".
// Return dst
template<typename StringType> StringType &formatD80(StringType &dst, const Double80 &x, StreamParameters &param) {
  StreamSize  prec = param.precision();
  FormatFlags flags = param.flags();

  if(prec < 0) {
    prec = MAXPRECISION;
  }

  String d80Str;
  if(!isfinite(x)) {
    TCHAR tmp[100];
    d80Str = formatUndefined(tmp, _fpclass(x), (flags & ios::uppercase), true);
    flags &= ~ios::hexfloat;
    param.flags(flags);
  } else if(x.isZero()) {
    formatZero(d80Str, prec, flags);
  } else if((flags & ios::floatfield) == ios::hexfloat) { // x defined && x != 0
    formatHex(d80Str, x, prec, flags);
  } else {
    D80StringFields(x).formatFloat(d80Str, prec, flags);
  } // x defined && x != 0
  return formatFilledFloatField(dst, d80Str, x.isNegative(), param);
}

template<typename OStreamType> OStreamType &putDouble80(OStreamType &out, const Double80 &x) {
  String s;
  out << formatD80(s, x, StreamParameters(out));
  if(out.flags() & ios::unitbuf) {
    out.flush();
  }
  return out;
}

template<typename IStreamType, typename CharType> IStreamType &getDouble80(IStreamType &in, Double80 &x) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const bool    hex   = ((in.flags() & ios::floatfield) == ios::hexfloat);
  RegexIStream &regex = hex 
                      ? (RegexIStream&)HexFloatValueStreamScanner::getInstance()
                      : (RegexIStream&)DecFloatValueStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    if(hex) insertHexPrefixIfMissing(buf);
    x = _tcstod80(buf.cstr(), NULL);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

}; // namespace Double80IO