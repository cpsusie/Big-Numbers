#include "pch.h"
#include "Double80Str.h"

namespace Double80Str {

using namespace std;

unsigned char hexCharLookup[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,
  0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0,10,11,12,13,14,15, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Assume x > 0
char *getDigitsStr(char *str, Double80 &x, int &e10) {
  const UINT64 tmp  = (UINT64)x;
  Double80     diff = x - tmp;          // diff >= 0
  if(diff.isZero()) {                   // diff == 0 => tmp == x => use ui64toa on tmp as result
    _ui64toa(tmp, str, 10);
    e10 += (int)strlen(str);
    return str;
  } else {                              // 0 < diff <= 1 => _MAXUI64 < x <= _MAXUI64+1
    _ui64toa(tmp, str, 10);
    e10 += (int)strlen(str);
    int diffInt;
    do {
      diff *= 10;
      diffInt = (int)diff;
    } while(diff != diffInt);
    char fraction[10];
    return strcat(str, _itoa(diffInt,fraction,10));
  }
}

static const TCHAR *hexPrefix = _T("0x");

void insertHexPrefixIfMissing(String &str) {
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


// Assume x != 0 and finite and precision >= 0. ignore sign of x
// Return dst
String &formatHex(String &dst, const Double80 &x, StreamSize prec, FormatFlags flags) {
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

// floor(MAX_UINT64 / 10) = 0x1999999999999999ui64 = 1.844.674.407.370.955.161
const Double80 &d80Maxui64q10 = *(Double80*)(void*)"\xC8\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x3B\x40";

#define MINEXPO10 -4910

// Assume m > 0, and expo10 approx = expo10(m)
// normalize m, so _UI64_MAX/10 < m <= _UI64_MAX and x = (negative?-1:1) * m * 10^(expo10-DBL80_DIG)
void normalizeValue(Double80 &m, int &expo10) {
  if(expo10 > 0) {
    const Double80 tmp = m / Double80::pow10(expo10);
    if(tmp >= d80Maxui64q10) {
      m = tmp;
    } else {
      expo10--;
      m /= Double80::pow10(expo10);
    }
  } else if(expo10 < 0) {
    if(expo10 >= MINEXPO10) { // take care of denormalized numbers
      const Double80 tmp = m * Double80::pow10(-expo10);
      if(tmp >= d80Maxui64q10) {
        m = tmp;
      } else {
        expo10--;
        m *= Double80::pow10(-expo10);
      }
    } else {              // use 2 steps to prevent overflow
      m *= Double80::pow10(-MINEXPO10);
      const Double80 tmp = m * Double80::pow10(MINEXPO10 - expo10);
      if(tmp >= d80Maxui64q10) {
        m = tmp;
      } else {
        expo10--;
        m *= Double80::pow10(MINEXPO10 - expo10);
      }
    }
  }
  if(m < d80Maxui64q10) {
    m *= 10;
    expo10--;
  }
}

}; // namespace Double80Str
