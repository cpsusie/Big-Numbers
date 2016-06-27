#include "stdafx.h"
#include "CppUnitTest.h"
#include <limits.h>

#ifdef IS64BIT
#include <Math/Int128.h>
#endif
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#ifdef verify
#undef verify
#endif
#define verify(expr) Assert::IsTrue(expr, _T(#expr))

namespace TestInt128 {		

  void OUTPUT(const TCHAR *format, ...) {
    va_list argptr;
    va_start(argptr, format);
    const String msg = vformat(format, argptr);
    va_end(argptr);
    Logger::WriteMessage(msg.cstr());
  }

  TEST_CLASS(TesInt128) {
    public:

#ifdef IS64BIT

    TEST_METHOD(Int128Comparators) {
      int              minI32   = _I32_MIN;
      int              maxI32   = _I32_MAX;
      UINT             maxUI32  = _UI32_MAX;
      __int64          minI64   = _I64_MIN;
      __int64          maxI64   = _I64_MAX;
      unsigned __int64 maxUI64  = _UI64_MAX;
      _int128          minI128  = _I128_MIN;
      _int128          maxI128  = _I128_MAX;
      _uint128         maxUI128 = _UI128_MAX;

      _int128 i1 = minI32;
      verify(i1 == minI32);
      _int128 i2 = maxI32;
      verify(i2 == maxI32);

      _int128 i3 = minI64;
      verify(i3 == minI64);
      _int128 i4 = maxI64;
      verify(i4 == maxI64);

      _int128 i5 = minI64;
      verify(i5 == minI64);
      i5--;
      verify(i5 < minI64);
      i5++;
      verify(i5 == minI64);
      verify(i5 <= minI64);
      verify(i5 >= minI64);
      i5++;
      verify(i5 > minI64);

      _int128 i6 = maxI64;
      verify(i6 == maxI64);
      i6++;
      verify(i6 > maxI64);
      i6--;
      verify(i6 == maxI64);
      verify(i6 <= maxI64);
      verify(i6 >= maxI64);
      i6--;
      verify(i6 < maxI64);

      _uint128 i7 = maxUI64;
      verify(i7 == maxUI64);
      i7++;
      verify(i7 > maxUI64);
      i7--;
      verify(i7 == maxUI64);
      verify(i7 <= maxUI64);
      verify(i7 >= maxUI64);
      i7--;
      verify(i7 < maxUI64);

      _int128 i8 = 0;
      verify(i8 == 0);
      verify(i8 >= 0);
      verify(i8 <= 0);
      i8--;
      verify(i8 <   0);
      verify(i8 == -1);
      verify(i8 >= -1);
      verify(i8 <= -1);
      i8++;
      verify(i8 == 0);
      verify(i8 >= 0);
      verify(i8 <= 0);

      i8++;
      verify(i8 >  0);
      verify(i8 == 1);
      verify(i8 >= 1);
      verify(i8 <= 1);


      _int128 i9 = maxI128;
      verify(i9 == maxI128);
      i9--;
      verify(i9 <  maxI128);
      verify(i9 <= maxI128);
      i9++;
      verify(i9 == maxI128);
      verify(i9 <= maxI128);
      verify(i9 >= maxI128);
      i9++;
      verify(i9 == minI128);
      verify(i9 < 0);
      i9--;
      verify(i9 == maxI128);

      _int128 i10 = minI128;
      verify(i10 == minI128);
      i10++;
      verify(i10 >  minI128);
      verify(i10 >= minI128);
      i10--;
      verify(i10 == minI128);
      verify(i10 <= minI128);
      verify(i10 >= minI128);
      i10--;
      verify(i10 == maxI128);
      verify(i10 > 0);
      i10++;
      verify(i10 == minI128);

      _uint128 i11 = maxUI128;
      verify(i11 == maxUI128);
      i11--;
      verify(i11 <  maxUI128);
      verify(i11 <= maxUI128);
      i11++;
      verify(i11 == maxUI128);
      verify(i11 <= maxUI128);
      verify(i11 >= maxUI128);
      i11++;
      verify(i11 == 0);
      verify(i11 <= 0);
      verify(i11 >= 0);
      i11--;
      verify(i11 == maxUI128);

    }

    TEST_METHOD(Int128ArithmethicOperators) {
      _uint128 x1("0xffffffffffffffffffffffffffffffff");
      _uint128 res1 = x1 + 1;
      verify(res1 == 0);

      _uint128 x2(  "12345678901234567890123456789012345");
      _uint128 y2(  "23456789012345678901234567890123456");
      _uint128 z2 = x2 + y2;
      _uint128 res2("35802467913580246791358024679135801");
      verify(z2 == res2);

      _uint128 z3 = y2 - x2;
      const char *str3 = "11111110111111111011111111101111111";
      _uint128 res3(str3);

      verify(z3 == res3);
      char buf3[300];
      _ui128toa(z3, buf3, 10);
      verify(strcmp(str3, buf3) == 0);

      _uint128 x4(  "340282366920938463463374607431768211455");
      verify(x4 == _UI128_MAX);
      _uint128 y4(  "54678423345639783523445");
      _uint128 q4 = x4 / y4;
      _uint128 r4 = x4 % y4;
      _uint128 z4 = q4 * y4 + r4;
      verify(r4 < y4);
      _uint128 x4a = (q4 + 1) * y4;
      _int128 d4 = x4 - x4a;
      verify((d4 < 0) && (-d4 < r4));
      verify(z4 == x4);

      _int128 x5("170141183460469231731687303715884105727");
      verify(x5 == _I128_MAX);
      _int128 y5(  "54678423345639783523445");
      _int128 x5copy(x5);
      _int128 y5copy(y5);
      _int128 q5 = x5 / y5;
      _int128 q5copy(q5);
      verify((x5 == x5copy) && (y5 == y5copy));
      _int128 r5 = x5 % y5;
      _int128 r5copy(r5);
      verify((x5 == x5copy) && (y5 == y5copy));
      _int128 z5 = q5 * y5 + r5;
      verify((x5 == x5copy) && (y5 == y5copy));
      verify((y5 == y5copy) && (q5 == q5copy) && (r5 == r5copy));
      verify(r5 < y5);
      verify(z5 == x5);

      _int128  x6( "0x80000000000000000000000000000000");
      _uint128 ux6("0x80000000000000000000000000000000");
      verify(x6 == _I128_MIN);
      _int128 y6(  "-54678423345639783523445");

    }

#endif // IS64BIT

  };
}
