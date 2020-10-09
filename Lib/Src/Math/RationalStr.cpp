#include "pch.h"
#include "RationalStr.h"

namespace RationalStr {

using namespace std;

// assume !r.isInteger()
// Return dst
String &formatRational(String &dst, const Rational &r, StreamParameters &param) {
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

}; // namespace RationalStr
