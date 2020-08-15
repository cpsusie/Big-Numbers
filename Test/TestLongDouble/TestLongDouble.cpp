#include "stdafx.h"
#include <MathUtil.h>
#include <InputValue.h>
#include <Math/Double80.h>

#define FSZ(n) format1000(n).cstr()

int main(int argc, TCHAR **argv) {
//  _tprintf(_T("sizeof(long double):%s\n"), FSZ(sizeof(long double)));
//  _tprintf(_T("sizeof(Double80:%s\n"), FSZ(sizeof(Double80)));

  unsigned __int64 ui64max = _UI64_MAX;
  Double80         dui64   = ui64max;
  unsigned __int64 rui64   = (UINT64)dui64;

  for (;;) {
    double x = inputValue<double>(_T("Enter x:"));
//    double y = inputValue<double>(_T("Enter y:"));
    Double80 x80 = x; // , y80 = y;
//    Double80 z80 = pow(x80, y80);
    Double80 z80 = exp2(x80);
    double   z64 = (double)z80;
    _tprintf(_T("exp2(%lg)=%le\n"), x, z64);
  }
  Double80 x, y;
  unsigned long l1 = 1;
  x = 1;
  y = 2.3;
  Double80 z   = x + y;
  double   z64 = (double)z;
  _tprintf(_T("x+y:%le\n"), z64);

  x   = 9.7;
  y   = 4;
  z   = fmod(x, y);
  z64 = (double)z;

  x = M_PI;
  x *= 2;
  x /= 3;
  double   x64 = (double)x;
  Double80 c   = x, s;
  sincos(c, s);
  double   c64 = (double)c;
  double   s64 = (double)s;
  z   = cos(x);
  z64 = (double)z;

  double   e10    = 9.999e20;
  Double80 d1080  = e10;
  int      expo10 = Double80::getExpo10(d1080);

  Double80   exp80  = exp(z);
  double     exp64  = (double)exp80;
  const long maxi32 = 0x7fffffff;
  Double80   zi32(maxi32);
  const long i32_1  = (INT)zi32;

  const unsigned long    ui32_a1    = maxi32;
  Double80               dui32_a(ui32_a1);
  const unsigned long    ui32_a2    = (UINT)dui32_a;

  const unsigned long    ui32_b1    = (ULONG)maxi32 + 1;
  Double80               dui32_b(ui32_b1);
  const unsigned long    ui32_b2    = (UINT)dui32_b;

  const __int64          maxi64     = 0x7fffffffffffffffui64;
  Double80               di64(maxi64);
  const __int64          i64_1      = (INT64)di64;

  const unsigned __int64 ui64_a1    = maxi64;
  Double80               dui64_a(ui64_a1);
  const unsigned __int64 ui64_a2    = (UINT64)dui64_a;

  const unsigned __int64 ui64_b1    = (unsigned __int64)maxi64 + 1;
  Double80               dui64_b(ui64_b1);
  const unsigned __int64 ui64_b2    = (UINT64)dui64_b;

  const float            f1         = 1.23456f;
  Double80               zf(f1);
  const float            f2         = (float)zf;

  return 0;
}
