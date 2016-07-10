#include "stdafx.h"
#include "CppUnitTest.h"
#include <limits.h>

#ifdef IS64BIT
#include <Math/Int128.h>
#endif
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;

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

  template<class OUTSTREAM> OUTSTREAM &setFormat(OUTSTREAM &os, ios::_Fmtflags baseFlag, unsigned int width, int showPos, int showBase, int uppercase, ios::_Fmtflags adjustFlag) {
    os.setf(baseFlag  , ios::basefield);
    os.setf(adjustFlag, ios::adjustfield);
    os.width(width);
    if (showBase) {
      os.setf(ios::showbase);
    }
    if (showPos) {
      os.setf(ios::showpos);
    }
    if (uppercase) {
      os.setf(ios::uppercase);
    }
    return os;
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

      _uint128 ui17 = maxUI64;
      verify(ui17 == maxUI64);
      ui17++;
      verify(ui17 > maxUI64);
      ui17--;
      verify(ui17 == maxUI64);
      verify(ui17 <= maxUI64);
      verify(ui17 >= maxUI64);
      ui17--;
      verify(ui17 < maxUI64);

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

      _uint128 ui11 = maxUI128;
      verify(ui11 == maxUI128);
      ui11--;
      verify(ui11 <  maxUI128);
      verify(ui11 <= maxUI128);
      ui11++;
      verify(ui11 == maxUI128);
      verify(ui11 <= maxUI128);
      verify(ui11 >= maxUI128);
      ui11++;
      verify(ui11 == 0);
      verify(ui11 <= 0);
      verify(ui11 >= 0);
      ui11--;
      verify(ui11 == maxUI128);


      _uint128 ui12 = ui11--; // post decrement
      verify(ui12 == ui11 + 1);
      ui12 = ui11;
      _uint128 ui13 = --ui11; // pre-decrement
      verify((ui13 == ui12 - 1) && (ui13 == ui11));

      _uint128 ui14 = ui11++; // post increment;
      verify(ui14 == ui11 - 1);
      ui14 = ui11;
      _uint128 ui15 = ++ui11; // pre-increment
      verify((ui15 == ui14 + 1) && (ui15 == ui11));

      _int128 i15 = maxUI64;
      i15++;
      _int128 ei15("0x10000000000000000");
      verify(i15 == ei15);
      i15--;
      verify(i15 == maxUI64);

    } // Int128Comparators

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

// ---------------------------------------------------------------

      _int128 x5p("170141183460469231731687303715884105727");
      _int128 x5n("-170141183460469231731687303715884105728");
      verify(x5p == _I128_MAX);
      verify(x5n == _I128_MIN);
      _int128 y5p(  "54678423345639783523445");
      _int128 y5n( "-54678423345639783523445");
      _int128 x5pcopy(x5p);
      _int128 y5pcopy(y5p);
      _int128 x5ncopy(x5n);
      _int128 y5ncopy(y5n);

      _int128 s5np = x5n + y5p;
      _int128 s5npcopy(s5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (s5np <= 0) && (s5np > x5n));

      _int128 d5nn = x5n - y5n;
      _int128 d5nncopy(d5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (d5nn <= 0) && (d5nn > x5n));

      _int128 s5pn = x5p + y5n;
      _int128 s5pncopy(s5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (s5pn >= 0) && (s5pn < x5p));

      _int128 d5pp = x5p - y5p;
      _int128 d5ppcopy(d5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (d5pp >= 0) && (d5pp < x5p));

      s5np -= y5p;
      verify((s5np == x5ncopy) && (y5p == y5pcopy));

      d5nn += y5n;
      verify((d5nn == x5ncopy) && (y5n == y5ncopy));

      s5pn -= y5n;
      verify((s5pn == x5pcopy) && (y5n == y5ncopy));

      d5pp += y5p;
      verify((d5pp == x5pcopy) && (y5p == y5pcopy));

// ---------------------------------------------------------------

// ---------------------------------------------------------------

      _int128 q5pp = x5p / y5p;
      _int128 q5ppcopy(q5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (q5pp >= 0));

      _int128 q5pn = x5p / y5n;
      _int128 q5pncopy(q5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (q5pn <= 0));

      _int128 q5np = x5n / y5p;
      _int128 q5npcopy(q5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (q5np <= 0));

      _int128 q5nn = x5n / y5n;
      _int128 q5nncopy(q5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (q5nn >= 0));

// ---------------------------------------------------------------

      _int128 r5pp = x5p % y5p;
      _int128 r5ppcopy(r5pp);
      verify((x5p == x5pcopy) && (y5p == y5pcopy) && (r5pp >= 0) && (r5pp < y5p));

      _int128 r5pn = x5p % y5n;
      _int128 r5pncopy(r5pn);
      verify((x5p == x5pcopy) && (y5n == y5ncopy) && (r5pn >= 0) && (r5pn < y5p));

      _int128 r5np = x5n % y5p;
      _int128 r5npcopy(r5np);
      verify((x5n == x5ncopy) && (y5p == y5pcopy) && (r5np <= 0) && (r5np > y5n));

      _int128 r5nn = x5n % y5n;
      _int128 r5nncopy(r5nn);
      verify((x5n == x5ncopy) && (y5n == y5ncopy) && (r5nn <= 0) && (r5nn > y5n));

// ---------------------------------------------------------------

      _int128 z5pp = q5pp * y5p + r5pp;
      verify((z5pp == x5pcopy) && (y5p == y5pcopy) && (q5pp == q5ppcopy) && (r5pp == r5ppcopy));

      _int128 z5pn = q5pn * y5n + r5pn;
      verify((z5pn == x5pcopy) && (y5n == y5ncopy) && (q5pn == q5pncopy) && (r5pn == r5pncopy));

      _int128 z5np = q5np * y5p + r5np;
      verify((z5np == x5ncopy) && (y5p == y5pcopy) && (q5np == q5npcopy) && (r5np == r5npcopy));

      _int128 z5nn = q5nn * y5n + r5nn;
      verify((z5nn == x5ncopy) && (y5n == y5ncopy) && (q5nn == q5nncopy) && (r5nn == r5nncopy));

// ---------------------------------------------------------------

      _int128 x5qp = x5p;
      _int128 x5qn = x5n;

      x5qp /= y5p;
      verify((x5qp == q5pp) && (y5p == y5pcopy));

      x5qn /= y5p;
      verify((x5qn == q5np) && (y5p == y5pcopy));

      x5qp *= y5p;
      verify((x5p - x5qp == r5pp) && (y5p == y5pcopy));

      x5qn *= y5p;
      verify((x5n - x5qn == r5np) && (y5p == y5pcopy));

      x5qp = x5p;
      x5qn = x5n;

      x5qp /= y5n;
      verify((x5qp == q5pn) && (y5n == y5ncopy));

      x5qn /= y5n;
      verify((x5qn == q5nn) && (y5n == y5ncopy));

      x5qp *= y5n;
      verify((x5p - x5qp == r5pn) && (y5n == y5ncopy));

      x5qn *= y5n;
      verify((x5n - x5qn == r5nn) && (y5n == y5ncopy));

// ---------------------------------------------------------------

      _int128 x5rp = x5p;
      _int128 x5rn = x5n;

      x5rp %= y5p;
      verify((x5rp == r5pp) && (y5p == y5pcopy));

      x5rn %= y5p;
      verify((x5rn == r5np) && (y5p == y5pcopy));

      x5rp = x5p;
      x5rn = x5n;

      x5rp %= y5n;
      verify((x5rp == r5pn) && (y5n == y5ncopy));

      x5rn %= y5n;
      verify((x5rn == r5nn) && (y5n == y5ncopy));

// ---------------------------------------------------------------

      _int128  x6( "0x80000000000000000000000000000000");
      _uint128 ux6("0x80000000000000000000000000000000");
/*
      TCHAR bx6[1200], bimin[200];
      _i128tow(x6, bx6, 16);
      _i128tow(_I128_MIN, bimin, 16);
      OUTPUT(_T("x6:%s"), bx6);
      OUTPUT(_T("Imin:%s"), bimin);
*/
      verify(x6 == _I128_MIN);
      _int128 y6(  "-54678423345639783523445");

    } // Int128ArithmethicOperators

    TEST_METHOD(Int128bitOperators) {
      _int128 x1 = 0;
      x1 = ~x1;
      verify(x1 == -1);
      x1 = x1 >> 2;
      verify(x1 == -1);
      x1 = x1 << 3;
      verify(x1 == -8);

      _uint128 x2 = 0;
      x2 = ~x2;
      verify(x2 == _UI128_MAX);

      x2 = x2 >> 3;
      verify(x2 == _UI128_MAX / 8);
      x2 = x2 << 3;
      verify(x2 == _UI128_MAX - 7);
      x2 = x2 | 7;
      verify(x2 == _UI128_MAX);

      _int128 b1("0x123456789abcdef0123456789abcdef");
      _int128 b2("0x7777777777777777777777777777777");
      _int128 b3("0x46328ab5cdf43a89b3c819bf6483219");
      _int128 ea("0x1234567012345670123456701234567");
      verify((b1 & b2) == ea);
      _int128 eo("0x7777777ffffffff77777777ffffffff");
      verify((b1 | b2) == eo);
      
      _int128 x13 = b1 ^ b3;
      _int128 b1mb3 = b1 & ~b3;
      _int128 b3mb1 = b3 & ~b1;
      _int128 symDif13 = b1mb3 | b3mb1;
      verify(x13 == symDif13);

    } // Int128ShiftOperators

    TEST_METHOD(Int128bitStreamOperators) {
      try {
        CompactArray<_int128> sa;
        CompactArray<_uint128> ua;

        for (_int128 x = 0; x >= 0; x = (x + 1) * 3) { // add some positive test-numbers
          sa.add(x);
        }
        for (_int128 x = 0; x <= 0; x = (x - 1) * 3) { // add some negative test-numbers
          sa.add(x);
        }
        for (_uint128 x = 0;; x = (x + 1) * 5) {       // add unsigned test-numbers
          if ((ua.size() > 1) && (x < ua.last())) break;
          ua.add(x);
        }

        ios::_Fmtflags baseFlags[] = {
          ios::dec
         ,ios::hex
         ,ios::oct
        };
        ios::_Fmtflags adjustFlags[] = {
          ios::left
         ,ios::right
         ,ios::internal
        };

        // try all(almost) combinations of output format flags
        for (int b = 0; b < ARRAYSIZE(baseFlags); b++) {
          const ios::_Fmtflags baseFlag   = baseFlags[b];
          int maxShowPos, maxShowBase, maxUpper;
          switch (baseFlag) {
          case ios::dec: maxShowPos = 1; maxShowBase = 0; maxUpper = 0; break;
          case ios::hex: maxShowPos = 0; maxShowBase = 1; maxUpper = 1; break;
          case ios::oct: maxShowPos = 0; maxShowBase = 1; maxUpper = 0; break;
          }
          for (int showPos = 1; showPos <= maxShowPos; showPos++) {
            for (int showBase = 0; showBase <= maxShowBase; showBase++) {
              for (int uppercase = 0; uppercase <= maxUpper; uppercase++) {
                for (int a = 0; a < ARRAYSIZE(adjustFlags); a++) {
                  for (unsigned int width = 0; width < 20; width += 3) {
                    ostringstream  ostr;
                    wostringstream wostr;
                    const ios::_Fmtflags adjustFlag = adjustFlags[a];

                    for (size_t i = 0; i < sa.size(); i++) { // write signed
                      setFormat<ostream>(ostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      setFormat<wostream>(wostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      ostr << sa[i] << "\n";
                      wostr << sa[i] << "\n";
                    }
                    for (size_t i = 0; i < ua.size(); i++) { // write unsigned
                      setFormat<ostream>(ostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      setFormat<wostream>(wostr, baseFlag, width, showPos, showBase, uppercase, adjustFlag);
                      ostr << ua[i] << "\n";
                      wostr << ua[i] << "\n";
                    }

                    string  str = ostr.str();
                    wstring wstr = wostr.str();

                    for (Tokenizer tok(wstr.c_str(), _T("\n")); tok.hasNext();) {
                      const String s = tok.next();
                      verify(s.length() >= width);
                      const TCHAR *np;

                      if (adjustFlag == ios::right) {
                        for (np = s.cstr(); *np == _T(' '); np++);
                        verify(s.last() != _T(' '));
                      }
                      else {
                        np = s.cstr();
                        verify(s[0] != _T(' '));
                      }
                      if (baseFlag == ios::dec) {
                        if (showPos) {
                          verify((np[0] == _T('-')) || (np[0] == _T('+')));
                        }
                      }
                      else if (showBase) {
                        verify(np[0] == _T('0'));
                        if (baseFlag == ios::hex) {
                          verify(np[1] == _T('x'));
                        }
                      }
                      if (uppercase) {
                        for (const TCHAR *cp = np; *cp; cp++) {
                          verify(!_istlower(*cp));
                        }
                      }
                      else {
                        for (const TCHAR *cp = np; *cp; cp++) {
                          verify(!_istupper(*cp));
                        }
                      }
                    }

                    istringstream  istr;
                    wistringstream wistr;

                    istr.str(str);
                    wistr.str(wstr);

                    for (size_t i = 0; i < sa.size(); i++) { // read signed
                      _int128 x;
                      istr.setf(baseFlag, ios::basefield);
                      istr >> x;
                      verify(x == sa[i]);

                      _int128 wx;
                      wistr.setf(baseFlag, ios::basefield);
                      wistr >> wx;
                      verify(wx == sa[i]);
                    }
                    for (size_t i = 0; i < ua.size(); i++) { // read unsigned
                      _uint128 x;
                      istr.setf(baseFlag, ios::basefield);
                      istr >> x;
                      verify(x == ua[i]);
                      _uint128 wx;
                      wistr.setf(baseFlag, ios::basefield);
                      wistr >> wx;
                      verify(wx == ua[i]);
                    }
                  } // for width=[0..20]
                } // for all AdjustFlags
              } // for lower/uppercase
            } // for all showBase
          } // for all showPos
        } // for all baseFlags
      }
      catch (Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
#endif // IS64BIT

  };
}
