#include "stdafx.h"
#include <Math/Int128.h>
#include <Timer.h>

String toString(const _uint128 &n, int radix=10) {
  char tmp[200];
  _ui128toa(n, tmp, radix);
  return tmp;
}

String toString(const _int128 &n, int radix=10) {
  char tmp[200];
  _i128toa(n, tmp, radix);
  return tmp;
}

#define RADIX 10
//#define RADIX 16

static _int128 inputInt128(TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  String msg = vformat(format, argptr);
  for (;;) {
    _tprintf(_T("%s"), msg.cstr());
    char line[1000];
    fgets(line, ARRAYSIZE(line), stdin);
    try {
      _int128 x(line);
      return x;
    }
    catch (Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
}


void loopSigned() {
  for (;;) {
    const _int128 x = inputInt128(_T("Enter x:"));
    const _int128 y = inputInt128(_T("Enter y:"));

    _int128 s = x + y;
    _int128 d = x - y;
    _int128 p = x * y;
    _int128 q = x / y;
    _int128 r = x % y;

    _tprintf(_T("x=%s\n"), toString(x).cstr());
    _tprintf(_T("y=%s\n"), toString(y).cstr());

    _tprintf(_T("x+y=s=%s\n"), toString(s).cstr());
    _tprintf(_T("x-y=d=%s\n"), toString(d).cstr());
    _tprintf(_T("x*y=p=%s\n"), toString(p).cstr());
    _tprintf(_T("x/y=q=%s\n"), toString(q).cstr());
    _tprintf(_T("x%%y=r=%s\n"), toString(r).cstr());

    _int128 c1 = (s + d) / _int128(2);
    _int128 c2 = (s - d) / _int128(2);
    _int128 c3 = q * y + r;

    _tprintf(_T("(s+d)/2=%s\n"), toString(c1).cstr());
    _tprintf(_T("(s-d)/2=%s\n"), toString(c2).cstr());
    _tprintf(_T("(q*y+r =%s\n"), toString(c3).cstr());
    _tprintf(_T("______________________________________________\n"));

    const bool lt = x < y;
    const bool le = x <= y;
    const bool gt = x > y;
    const bool ge = x >= y;

    _tprintf(_T("x<y:%s, x<=y:%s, x>y:%s, x>=y:%s\n\n")
      , boolToStr(lt), boolToStr(le), boolToStr(gt), boolToStr(ge));
  }
}

static _uint128 tryr8r9(const _uint128 &x, const _uint128 &y, const TCHAR *msgr8, const TCHAR *msgr9) {
  _tprintf(_T("msgr8:<%s>, msgr9:<%s>\n"), msgr8, msgr9);

  const _uint128 q = x / y;

  _tprintf(_T("msgr8:<%s>, msgr9:<%s>\n"), msgr8, msgr9);

  return q;
}

int main(int argc, TCHAR **argv) {

  const TCHAR *msgr8 = _T("register8 points to this");
  const TCHAR *msgr9 = _T("register9 points to this");
  const _uint128 x = 123456789;
  const _uint128 y = 1234;

  _uint128 q = tryr8r9(x, y, msgr8, msgr9);
  TCHAR buf[100];
  _i128tow(q, buf, 10);

  _tprintf(_T("q:%s\n"), buf);
  return 0;



  loopSigned();
  _uint128  x1(0xaaaaaaaabbbbbbbb, 0xfabcdef12345678);
  _uint128  x2 = 0xccccdddddddd;

  _uint128 x3 = x1 / x2;
  _uint128 x4 = x1 % x2;
  _uint128 x5 = x3 * x2 + x4;

  _tprintf(_T("x3.%016I64x:%016I64x (%016I64x%016I64x)\n"), x3.hi, x3.lo, x3.hi, x3.lo);
  _tprintf(_T("x4.%016I64x:%016I64x (%016I64x%016I64x)\n"), x4.hi, x4.lo, x4.hi, x4.lo);
  _tprintf(_T("x5.%016I64x:%016I64x (%016I64x%016I64x)\n"), x5.hi, x5.lo, x5.hi, x5.lo);

/*
  _int128 x("12345678901234567890123456");
  _int128 y("43215456543");

  _tprintf(_T("maxInt:%s (=0x%s)\n"), toString(x1, 10).cstr(), toString(x1, 16).cstr());
  _tprintf(_T("minInt:%s\n"), toString(x2, 10).cstr());
  _tprintf(_T("maxUInt:%s (=0x%s)\n"), toString(x3, 10).cstr(), toString(x3, 16).cstr());
  return 0;

  for (int i = 0; i < 129; i++) {
    _uint128 z = x >> i;
    _tprintf(_T("i:%3d, x>>%3d:%128s\n"), i, i, toString(z, 2).cstr());
  }
  for (int i = 0; i < 129; i++) {
    _uint128 z = x << i;
    _tprintf(_T("i:%3d, x<<%3d:%128s\n"), i, i, toString(z, 2).cstr());
  }
  return 0;

  String str = toString(x);

  _int128 s = x + y;
  _int128 d = x - y;
  _int128 p = x * y;
  _int128 q = x / y;
  _int128 r = x % y;

  _tprintf(_T("%-30s + %-30s = %30s\n"), toString(x,RADIX).cstr(), toString(y,RADIX).cstr(), toString(s).cstr());
  _tprintf(_T("%-30s - %-30s = %30s\n"), toString(x,RADIX).cstr(), toString(y,RADIX).cstr(), toString(d).cstr());
  _tprintf(_T("%-30s * %-30s = %30s\n"), toString(x,RADIX).cstr(), toString(y,RADIX).cstr(), toString(p).cstr());
  _tprintf(_T("%-30s / %-30s = %30s\n"), toString(x,RADIX).cstr(), toString(y,RADIX).cstr(), toString(q).cstr());
  _tprintf(_T("%-30s %% %-30s = %30s\n"), toString(x,RADIX).cstr(), toString(y,RADIX).cstr(), toString(r).cstr());
  return 0;
*/
}
