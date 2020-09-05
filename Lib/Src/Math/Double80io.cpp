#include "pch.h"
#include <StrStream.h>
#include <Math/Double80.h>

using namespace std;
using namespace OStreamHelper;
using namespace IStreamHelper;

// Assume x != 0 and finite and precision >= 0. ignore sign of x
// Return dst
static String &formatHex(String &dst, const Double80 &x, StreamSize prec, FormatFlags flags) {
  if(prec == 0) {
    prec = 6;
  }
  int e2;
  if(x.isZero()) {
    e2 = 0;
  } else {
    e2 = getExpo2(x);
    if(e2 < numeric_limits<Double80>::min_exponent) {
      e2 = numeric_limits<Double80>::min_exponent;
    }
  }
  // precision >= 1!!
  UINT64 sig       = getSignificand(x);
  UINT   intPart   = sig >> 63;
  size_t zeroCount = 0;
  TCHAR  fractionStr[100];
  sig <<= 1; // to get the fraction part only
  if(prec >= 16) {
    _stprintf(fractionStr, _T("%016I64x"), sig);
    zeroCount = (size_t)prec - 16;
  } else { // 0 < precision < 16.    => do rounding
    const UINT shift = (UINT)(15 - prec) * 4;
    if(shift) sig >>= shift;
    const BYTE lastDigit = sig & 0xf;
    sig >>= 4;
    if(lastDigit > 8) { // round up
      sig++;
      const UINT64 intPartMask = 1ui64 << (60 - shift);
      if(sig & intPartMask) {
        intPart++;
        sig &= ~intPartMask;
      }
    }
    _stprintf(fractionStr, _T("%0*I64x"), (int)prec, sig);
  }
  if(flags&ios::uppercase) {
    strToUpperCase(fractionStr);
  }
  dst += (_T('0') + intPart);
  addDecimalPoint(dst);
  dst += fractionStr;
  if(zeroCount) {
    addZeroes(dst, zeroCount);
  }
  addExponentChar(dst, flags);
  dst += format(_T("%+d"), e2);
  return dst;
}

class D80StringFields : public FloatStringFields {
public:
  D80StringFields(const Double80& x);
};

D80StringFields::D80StringFields(const Double80& x)
  : FloatStringFields(2)
{
  TCHAR tmp[50];
#if defined(_DEBUG)
  memset(tmp, 0, sizeof(tmp));
#endif
  init(d80tot(tmp, x));
}

#define MAXPRECISION numeric_limits<Double80>::digits10

// See Solution by Stephan T. Lavavej [MSFT] on
// https ://developercommunity.visualstudio.com/content/problem/1145155/wrong-output-in-x64-release-mode-c.html?childToView=1159350#comment-1159350
// For fixed and scientific, precision is the number of digits after the decimal point, ignoring any digits before the decimal point.
// For general(aka defaultfloat in C++), precision is the total number of digits, including those before the decimal point.
// Requesting showpoint is equivalent to the printf flag `#` which says "always print the decimal point, 
// even if there are no decimal digits to the right".
// Return dst
template<typename StringType> StringType &formatD80(StringType &dst, const Double80 &x, StreamParameters &param) {
  StreamSize  prec  = param.precision();
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

static const TCHAR *hexPrefix = _T("0x");

static void insertHexPrefixIfMissing(String &str) {
  TCHAR *c = str.cstr();
  while(iswspace(*c)) c++;
  if((*c == '-') || (*c == '+')) {
    c++;
  }
  if(_tcsnicmp(c, hexPrefix, 2) != 0) {
    const size_t pos = c - str.cstr();
    str.insert(pos, hexPrefix);
  }
}

template<typename IStreamType, typename CharType> IStreamType &getDouble80(IStreamType &in, Double80 &x) {
  IStreamScanner<IStreamType, CharType> scanner(in);

  const bool          hex   = ((in.flags() & ios::floatfield) == ios::hexfloat);
  const RegexIStream &regex = hex ? HexFloatValueStreamScanner::getInstance() : DecFloatValueStreamScanner::getInstance();
  String buf;
  if(regex.match(in, &buf) < 0) {
    scanner.endScan(false);
    return in;
  }
  try {
    if(hex) insertHexPrefixIfMissing(buf);
    x = _tcstod80(buf.cstr(),NULL);
  } catch(...) {
    scanner.endScan(false);
    throw;
  }
  scanner.endScan();
  return in;
}

istream &operator>>(istream &in, Double80 &x) {
  return getDouble80<istream,char>(in, x);
}

ostream &operator<<(ostream &out, const Double80 &x) {
  return putDouble80(out, x);
}

wistream &operator>>(wistream &in, Double80 &x) {
  return getDouble80<wistream,wchar_t>(in, x);
}

wostream &operator<<(wostream &out, const Double80 &x) {
  return putDouble80(out, x);
}
