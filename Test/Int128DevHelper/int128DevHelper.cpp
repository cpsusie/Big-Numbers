#include "stdafx.h"
#include <Math/Int128.h>
#include <Timer.h>

String ToString(const _uint128 &n, int radix=10) {
  char tmp[200];
  _ui128toa(n, tmp, radix);
  return tmp;
}

String ToString(const _int128 &n, int radix=10) {
  char tmp[200];
  _i128toa(n, tmp, radix);
  return tmp;
}

#define RADIX 10
//#define RADIX 16

template<class _itype> _itype vinputItype(_itype (*strToItype)(const char *,char **,int), const TCHAR *format, va_list argptr) {
  const String msg = vformat(format, argptr);
  for (;;) {
    _tprintf(_T("%s"), msg.cstr());
    char line[1000];
    fgets(line, ARRAYSIZE(line), stdin);
    try {
      errno = 0;
      const _itype x = strToItype(line, NULL, 10);
      if(errno == ERANGE) {
        throwException(_T("out of range"));
      }
      return x;
    } catch (Exception e) {
      _tprintf(_T("%s\n"), e.what());
    }
  }
}

static _int128 inputInt128(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const _int128 result = vinputItype<_int128>(_strtoi128, format, argptr);
  va_end(argptr);
  return result;
}

static _uint128 inputUint128(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  const _uint128 result = vinputItype<_uint128>(_strtoui128, format, argptr);
  va_end(argptr);
  return result;
}

template<class _itype> void loopItype(_itype (*inputItype)(const TCHAR *,...)) {
  for(;;) {
    const _itype x = inputItype(_T("Enter x:"));
    const _itype y = inputItype(_T("Enter y:"));

    _itype s = x + y;
    _itype d = x - y;
    _itype p = x * y;
    _itype q = x / y;
    _itype r = x % y;

    _tprintf(_T("x=%s\n"), ToString(x).cstr());
    _tprintf(_T("y=%s\n"), ToString(y).cstr());

    _tprintf(_T("x+y=s=%s\n"), ToString(s).cstr());
    _tprintf(_T("x-y=d=%s\n"), ToString(d).cstr());
    _tprintf(_T("x*y=p=%s\n"), ToString(p).cstr());
    _tprintf(_T("x/y=q=%s\n"), ToString(q).cstr());
    _tprintf(_T("x%%y=r=%s\n"), ToString(r).cstr());

    _itype c1 = (s + d) / _int128(2);
    _itype c2 = (s - d) / _int128(2);
    _itype c3 = q * y + r;

    _tprintf(_T("(s+d)/2=%s\n"), ToString(c1).cstr());
    _tprintf(_T("(s-d)/2=%s\n"), ToString(c2).cstr());
    _tprintf(_T("(q*y+r =%s\n"), ToString(c3).cstr());
    _tprintf(_T("______________________________________________\n"));

    const bool lt = x < y;
    const bool le = x <= y;
    const bool gt = x > y;
    const bool ge = x >= y;

    _tprintf(_T("x<y:%s, x<=y:%s, x>y:%s, x>=y:%s\n\n")
      , boolToStr(lt), boolToStr(le), boolToStr(gt), boolToStr(ge));
  }
}

#define loopSigned()   loopItype<_int128 >(inputInt128 )
#define loopUnsigned() loopItype<_uint128>(inputUint128)

int main(int argc, TCHAR **argv) {

  loopUnsigned();
  loopSigned();
  _uint128  x1(0xaaaaaaaabbbbbbbb, 0xfabcdef12345678);
  _uint128  x2 = 0xccccdddddddd;

  _uint128 x3 = x1 / x2;
  _uint128 x4 = x1 % x2;
  _uint128 x5 = x3 * x2 + x4;

  _tprintf(_T("x3.%016I64x:%016I64x (%016I64x%016I64x)\n"), HI64(x3), LO64(x3), HI64(x3), LO64(x3));
  _tprintf(_T("x4.%016I64x:%016I64x (%016I64x%016I64x)\n"), HI64(x4), LO64(x4), HI64(x4), LO64(x4));
  _tprintf(_T("x5.%016I64x:%016I64x (%016I64x%016I64x)\n"), HI64(x5), LO64(x5), HI64(x5), LO64(x5));

/*
  _int128 x("12345678901234567890123456");
  _int128 y("43215456543");

  _tprintf(_T("maxInt:%s (=0x%s)\n"), ToString(x1, 10).cstr(), ToString(x1, 16).cstr());
  _tprintf(_T("minInt:%s\n"), ToString(x2, 10).cstr());
  _tprintf(_T("maxUInt:%s (=0x%s)\n"), ToString(x3, 10).cstr(), ToString(x3, 16).cstr());
  return 0;

  for (int i = 0; i < 129; i++) {
    _uint128 z = x >> i;
    _tprintf(_T("i:%3d, x>>%3d:%128s\n"), i, i, ToString(z, 2).cstr());
  }
  for (int i = 0; i < 129; i++) {
    _uint128 z = x << i;
    _tprintf(_T("i:%3d, x<<%3d:%128s\n"), i, i, ToString(z, 2).cstr());
  }
  return 0;

  String str = ToString(x);

  _int128 s = x + y;
  _int128 d = x - y;
  _int128 p = x * y;
  _int128 q = x / y;
  _int128 r = x % y;

  _tprintf(_T("%-30s + %-30s = %30s\n"), ToString(x,RADIX).cstr(), ToString(y,RADIX).cstr(), ToString(s).cstr());
  _tprintf(_T("%-30s - %-30s = %30s\n"), ToString(x,RADIX).cstr(), ToString(y,RADIX).cstr(), ToString(d).cstr());
  _tprintf(_T("%-30s * %-30s = %30s\n"), ToString(x,RADIX).cstr(), ToString(y,RADIX).cstr(), ToString(p).cstr());
  _tprintf(_T("%-30s / %-30s = %30s\n"), ToString(x,RADIX).cstr(), ToString(y,RADIX).cstr(), ToString(q).cstr());
  _tprintf(_T("%-30s %% %-30s = %30s\n"), ToString(x,RADIX).cstr(), ToString(y,RADIX).cstr(), ToString(r).cstr());
  return 0;
*/
}
