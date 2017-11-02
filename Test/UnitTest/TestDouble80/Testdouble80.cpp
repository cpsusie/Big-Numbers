#include "stdafx.h"
#include "CppUnitTest.h"
#include <Math.h>
#include <float.h>
#include <limits.h>
#include <Math/MathFunctions.h>
#include <Math/Double80.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

using namespace std;

#define endl _T("\n")

static const double EPS = 3e-14;

namespace TestDouble80 {		

#include <UnitTestTraits.h>

  class LOG : public StrStream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), cstr());
    }
  };

  static double getRelativeError(double x64, const Double80 &x80) {
    const double relativeError = fabs(x64 - getDouble(x80));
    return (x64 == 0) ? relativeError : relativeError / x64;
  }

  static Double80 getRelativeError(const Double80 x, const Double80 &expected) {
    const Double80 absError = fabs(x - expected);
    return expected.isZero() ? absError : absError / expected;
  }

  static void testFunction(const String &name, Double80(*f80)(const Double80 &), double(*f64)(double), double low, double high) {
    double maxRelativeError = 0;
    const double step = (high - low) / 10;
    for(double x64 = low; x64 <= high; x64 += step) {
      const double y64 = f64(x64);

      const Double80 x80 = x64;
      const Double80 y80 = f80(x80);

      const double relativeError = getRelativeError(y64, y80);

      if(x64 != getDouble(x80) || relativeError > EPS) {
        LOG log;
        log  << _T("Function ") << name << _T(" failed.") << endl
             << _T("(x64,y64):(") << dparam(16) << x64 << _T(",") << dparam(16) << y64 << _T(").") << endl
             << _T("x64 - getDouble(x80):") << dparam(16) << (x64 - getDouble(x80)) << _T(".") << endl
             << _T("y64 - getdouble(y80):") << dparam(16) << (y64 - getDouble(y80)) << _T(".") << endl
             << _T("RelativeError:") << dparam(16) << relativeError << endl;
        verify(false);
      }
      if(relativeError > maxRelativeError) {
        maxRelativeError = relativeError;
      }
    }
    OUTPUT(_T("%-10s:Max relative error:%.16le"), name.cstr(), maxRelativeError);
  }

  static double Max(double x, double y) {
    return x > y ? x : y;
  }

  static double Min(double x, double y) {
    return x < y ? x : y;
  }

  static double fraction(double x) {
    if(x < 0) {
      return -fraction(-x);
    } else {
      return x - floor(x);
    }
  }

  static double pow10(double x) {
    return pow(10, x);
  }

  static double pow2(double x) {
    return pow(2, x);
  }

  static void checkResult(double x64, Double80 x80, TCHAR *op, double tolerance = EPS) {
    const double relativeError = getRelativeError(x64, x80);
    if(relativeError > tolerance) {
      OUTPUT(_T("operator %s failed. x64=%20.16le x80=%s. Relative error:%le"), op, x64, toString(x80).cstr(), relativeError);
      verify(false);
    }
  }

  static void checkResult(double x64, double y64, bool cmp64, Double80 x80, Double80 y80, bool cmp80, TCHAR *relation) {
    if(cmp64 != cmp80) {
      OUTPUT(_T("Relation %s failed. %20.16le %s %20.16le = %s. %s %s %s = %s")
              , relation
              , x64, relation, y64, boolToStr(cmp64)
              , toString(x80).cstr(), relation, toString(y80).cstr(), boolToStr(cmp80));
      verify(false);
    }
  }

#define CHECKOPERATOR(op,allowZero,x64,y1,x80,y2,...)                                                   \
  if(allowZero || (((y1 )!=0) && ((y2 )!=0))) checkResult((x64 op y1),(x80 op y2),_T(#op),__VA_ARGS__); \
  if(allowZero || (((x64)!=0) && ((x80)!=0))) checkResult((y1 op x64),(y2 op x80),_T(#op),__VA_ARGS__)

#define CHECKASSIGNOPERATOR(op,allowZero,y1,y2) \
  if(allowZero || (((y1)!=0) && ((y2)!=0))) {   \
    double   s64 = z64;                         \
    Double80 s80 = z80;                         \
    s64 op (y1);                                \
    s80 op (y2);                                \
    checkResult(s64,s80,_T(#op));               \
  }

#define CHECKRELATION(r,y1,y2)                                               \
  checkResult(x64       ,(double)y1,x64  r (y1),x80,y2 , x80 r (y2),_T(#r)); \
  checkResult((double)y1,x64       ,(y1) r x64 ,y2 ,x80,(y2) r  x80,_T(#r))

#define testOperatorI16(   op,allowZero)    CHECKOPERATOR(op,allowZero,x64,   (short  )y64,x80,  (short )y64)
#define testOperatorUI16(  op,allowZero)  { CHECKOPERATOR(op,allowZero,x64,   (USHORT )y64,x80,  (USHORT)y64); \
                                            const USHORT _ui16 = (USHORT )(y64+_I16_MAX);                      \
                                            CHECKOPERATOR(op,allowZero,x64,          _ui16,x80,        _ui16); \
                                          }

#define testOperatorI32(   op,allowZero)    CHECKOPERATOR(op,allowZero,x64,   (int    )y64,x80,  (int   )y64)
#define testOperatorUI32(  op,allowZero)  { CHECKOPERATOR(op,allowZero,x64,   (UINT32 )y64,x80,  (UINT32)y64); \
                                            const UINT32 _ui32 = (UINT32 )(y64+_I32_MAX);                      \
                                            CHECKOPERATOR(op,allowZero,x64,          _ui32,x80,        _ui32); \
                                          }

#define testOperatorI64(   op,allowZero)    CHECKOPERATOR(op,allowZero,x64,   (INT64  )y64,x80,  (INT64 )y64)
#define testOperatorUI64(  op,allowZero)  { CHECKOPERATOR(op,allowZero,x64,   (UINT64 )y64,x80,  (UINT64)y64); \
                                            const UINT64 _ui64 = (UINT64 )(y64+_I64_MAX);                      \
                                            CHECKOPERATOR(op,allowZero,x64,          _ui64,x80,        _ui64); \
                                          }

#define testOperatorFloat( op,allowZero)    CHECKOPERATOR(op,allowZero,x32,            y32,x80,          y32, 1e-6)
#define testOperatorDouble(op,allowZero)    CHECKOPERATOR(op,allowZero,x64,            y64,x80,          y64)
#define testOperatorD80(   op,allowZero)    CHECKOPERATOR(op,allowZero,x64,            y64,x80,          y80)


#define testAssignOpI16(   op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,   (short  )y64,  (short )y64)
#define testAssignOpUI16(  op,allowZero)  { CHECKASSIGNOPERATOR(op,allowZero,   (USHORT )y64,  (USHORT)y64); \
                                            const USHORT _ui16 = (USHORT )(y64+_I16_MAX);                    \
                                            CHECKASSIGNOPERATOR(op,allowZero,          _ui16,        _ui16); \
                                          }

#define testAssignOpI32(   op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,   (int    )y64,  (int   )y64)
#define testAssignOpUI32(  op,allowZero)  { CHECKASSIGNOPERATOR(op,allowZero,   (UINT32 )y64,  (UINT32)y64); \
                                            const UINT32 _ui32 = (UINT32 )(y64+_I32_MAX);                    \
                                            CHECKASSIGNOPERATOR(op,allowZero,          _ui32,        _ui32); \
                                          }

#define testAssignOpI64(   op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,   (INT64  )y64,  (INT64 )y64)
#define testAssignOpUI64(  op,allowZero)  { CHECKASSIGNOPERATOR(op,allowZero,   (UINT64 )y64,  (UINT64)y64); \
                                            const UINT64 _ui64 = (UINT64 )(y64+_I64_MAX);                    \
                                            CHECKASSIGNOPERATOR(op,allowZero,          _ui64,        _ui64); \
                                          }

#define testAssignOpFloat( op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,            y32,          y32)
#define testAssignOpDouble(op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,            y64,          y64)
#define testAssignOpD80(   op,allowZero)    CHECKASSIGNOPERATOR(op,allowZero,            y64,          y80)


#define testRelationI16(   op)    CHECKRELATION(op,   (short  )y64,  (short )y64)
#define testRelationUI16(  op)  { CHECKRELATION(op,   (USHORT )y64,  (USHORT)y64); \
                                  const USHORT _ui16 = (USHORT )(y64+_I16_MAX);    \
                                  CHECKRELATION(op,          _ui16,        _ui16); \
                                }

#define testRelationI32(   op)    CHECKRELATION(op,   (int    )y64,  (int   )y64)
#define testRelationUI32(  op)  { CHECKRELATION(op,   (UINT32 )y64,  (UINT32)y64); \
                                  const UINT32 _ui32 = (UINT32 )(y64+_I32_MAX);    \
                                  CHECKRELATION(op,          _ui32,        _ui32); \
                                }

#define testRelationI64(   op)    CHECKRELATION(op,   (INT64  )y64,  (INT64 )y64)
#define testRelationUI64(  op)  { CHECKRELATION(op,   (UINT64 )y64,  (UINT64)y64); \
                                  const UINT64 _ui64 = (UINT64 )(y64+_I64_MAX);    \
                                  CHECKRELATION(op,          _ui64,        _ui64); \
                                }

#define testRelationFloat( op)    CHECKRELATION(op,            y32,          y32)
#define testRelationDouble(op)    CHECKRELATION(op,            y64,          y64)
#define testRelationD80(   op)    CHECKRELATION(op,            y64,          y80)

#define testOperator(op,allowZero)  \
  testOperatorI16(   op,allowZero); \
  testOperatorUI16(  op,allowZero); \
  testOperatorI32(   op,allowZero); \
  testOperatorUI32(  op,allowZero); \
  testOperatorI64(   op,allowZero); \
  testOperatorUI64(  op,allowZero); \
  testOperatorFloat( op,allowZero); \
  testOperatorDouble(op,allowZero); \
  testOperatorD80(   op,allowZero)

#define testAssignOperator(op,allowZero) \
  testAssignOpI16(   op,allowZero);      \
  testAssignOpUI16(  op,allowZero);      \
  testAssignOpI32(   op,allowZero);      \
  testAssignOpUI32(  op,allowZero);      \
  testAssignOpI64(   op,allowZero);      \
  testAssignOpUI64(  op,allowZero);      \
  testAssignOpFloat( op,allowZero);      \
  testAssignOpDouble(op,allowZero);      \
  testAssignOpD80(   op,allowZero)

#define testRelation(r )  \
  testRelationI16(   r ); \
  testRelationUI16(  r ); \
  testRelationI32(   r ); \
  testRelationUI32(  r ); \
  testRelationI64(   r ); \
  testRelationUI64(  r ); \
  testRelationFloat( r ); \
  testRelationDouble(r ); \
  testRelationD80(   r )


static void testFunction(const String &name, Double80(*f80)(const Double80 &, const Double80 &), double(*f64)(double, double), double low1, double high1, double low2, double high2) {
    double maxRelativeError = 0;
    const double step1 = (high1 - low1) / 10;
    const double step2 = (high2 - low2) / 10;
    for(double x64 = low1; x64 <= high1; x64 += step1) {
      for(double y64 = low2; y64 <= high2; y64 += step2) {
        double         z64 = f64(x64, y64);
        const Double80 x80 = x64;
        const Double80 y80 = y64;
        const float    x32 = (float)x64;
        const float    y32 = (float)y64;
        Double80       z80 = f80(x80, y80);

        const double relativeError = getRelativeError(z64, z80);

        if(x64 != getDouble(x80) || y64 != getDouble(y80) || relativeError > EPS) {
          LOG log;
          log << _T("Function ") << name << _T(" failed.") << endl
              << _T("(x64,y64,z64):(") << dparam(16) << x64 << _T(",") << dparam(16) << y64 << _T(",") << dparam(16) << z64 << _T(").") << endl
              << _T("x64 - getDouble(x80):") << dparam(16) << (x64 - getDouble(x80)) << _T(".") << endl
              << _T("y64 - getdouble(y80):") << dparam(16) << (y64 - getDouble(y80)) << _T(".") << endl
              << _T("z64 - getdouble(z80):") << dparam(16) << (z64 - getDouble(z80)) << _T(".") << endl
              << _T("RelativeError:") << dparam(16) << relativeError << endl;
          verify(false);
        }
        if(relativeError > maxRelativeError) {
          maxRelativeError = relativeError;
        }

        testOperator(+,true );
        testOperator(-,true );
        testOperator(*,true );
        testOperator(/,false);

        testAssignOperator(+= ,true );
        testAssignOperator(-= ,true );
        testAssignOperator(*= ,true );
        testAssignOperator(/= ,false);

        testRelation(== );
        testRelation(!= );
        testRelation(<= );
        testRelation(>= );
        testRelation(<  );
        testRelation(>  );

      }
    }
    OUTPUT(_T("%-10s:Max relative error:%.16le"), name.cstr(), maxRelativeError);
  }

  static double(*currentRefFunction)(const double &, const double &);
  static double rf(double x, double y) {
    return currentRefFunction(x, y);
  }

  static void testFunction(const String &name, Double80(*f80)(const Double80 &, const Double80 &), double(*f64)(const double &, const double &), double low1, double high1, double low2, double high2) {
    currentRefFunction = f64;
    testFunction(name, f80, rf, low1, high1, low2, high2);
  }

	TEST_CLASS(TestDouble80) {
    public:

    TEST_METHOD(Double80Testio) {
      long     i32 = 0;
      double   d64 = 0;
      Double80 d80 = i32;
      String   s64 = toString(d64);
      String   s80 = toString(d80);
      verify(s64 == s80);

      short i16 = _I16_MAX;
      d64 = i16;
      d80 = i16;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      i16 = _I16_MIN;
      d64 = i16;
      d80 = i16;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      USHORT ui16 = _UI16_MAX;
      d64 = ui16;
      d80 = ui16;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      i32 = _I32_MAX;
      d64 = i32;
      d80 = i32;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      i32 = _I32_MIN;
      d64 = i32;
      d80 = i32;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      ULONG ui32 = _UI32_MAX;
      d64 = ui32;
      d80 = ui32;
      s64 = toString(d64);
      s80 = toString(d80);
      verify(s64 == s80);

      INT64 i64 = _I64_MIN;
      d80 = i64;
      s64 = toString(i64);
      s80 = toString(d80, 0, 0, ios::fixed);
      verify(substr(s64, 1, 17) == substr(s80, 1, 17));

      i64 = _I64_MAX;
      d80 = i64;
      s64 = toString(i64);
      s80 = toString(d80, 0, 0, ios::fixed);
      verify(substr(s64, 0, 17) == substr(s80, 0, 17));

      UINT64 ui64 = _UI64_MAX;
      d80 = ui64;
      s64 = toString(ui64);
      s80 = toString(d80, 0, 0, ios::fixed);
      verify(substr(s64, 0, 17) == substr(s80, 0, 17));

      ui64 = (UINT64)_I64_MAX + 10;
      d80 = ui64;
      s64 = toString(i64);
      s80 = toString(d80, 0, 0, ios::fixed);
      verify(substr(s64, 0, 17) == substr(s80, 0, 17));

      for(int i = 1; i >= -1; i -= 2) {
        for(double d64 = 1e-100*i; fabs(d64) < 1e100; d64 *= 1.1) {
          s64 = toString(d64, 13, 21, ios::scientific);
          d80 = d64;
          s80 = toString(d80, 13, 21, ios::scientific);
          if(s64 != s80) {
            OUTPUT(_T("d64:%23.15le -> \"%s\""), d64, s64.cstr());
            OUTPUT(_T("s80:\"%s\" toString(d80,18):\"%s\"\n"), s80.cstr(), toString(d80, 18, 0, ios::scientific).cstr());
          }
        }
      }

      const Double80 pi = Double80::M_PI;

      const Double80 diff = pi - Double80("3.14159265358979324");
      verify(fabs(diff) < 1e-17);
    }

    static double testRound(double x64, int dec) {
      const Double80 x80 = x64;
      const double   y64 = round(x64, dec);
      const Double80 y80 = round(x80, dec);

      const double relativeError = getRelativeError(y64, y80);

      if(x64 != getDouble(x80) || relativeError > EPS) {
        OUTPUT(_T("Function:%s failed"), _T("round"));
        OUTPUT(_T("x64 - getDouble(x80):%le"), x64 - getDouble(x80));
        OUTPUT(_T("y64 - getDouble(y80):%le"), y64 - getDouble(y80));
        OUTPUT(_T("RelativeError:%le\n"), relativeError);
        verify(false);
      }
      return relativeError;
    }

    TEST_METHOD(Double80MeasureExpo10) {
      const double startTime = getProcessTime();
      const Double80 stepFactor = 1.0012345;
      int count = 0;
      for(Double80 x = Double80::DBL80_MIN; !isNan(x); x *= stepFactor) {
        const int d = Double80::getExpo10(x);
        count++;
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e3 / count;

      OUTPUT(_T("TimeMeasure on Double80::getExpo10(): count = %d, Time = %.3le msec"), count, timeUsage);
    }

    void testAllCast(double d64) {
      Double80 d80 = d64;

      const int    i1_32  = (int)d64;
      const int    i2_32  = getInt(d80);
      verify(i2_32  == i1_32 );

      const UINT   ui1_32 = (UINT)d64;
      const UINT   ui2_32 = getUint(d80);
      verify(ui2_32 == ui1_32);

      const short  s1_16 = (short)d64;
      const short  s2_16 = getShort(d80);
      verify(s2_16  == s1_16 );

      const USHORT us1_16 = (USHORT)d64;
      const USHORT us2_16 = getUshort(d80);
      verify(us2_16 == us1_16);

      const long   l1_32  = (long)d64;
      const long   l2_32  = getLong(d80);
      verify(l2_32  == l1_32 );

      const ULONG  ul1_32 = (ULONG)d64;
      const ULONG  ul2_32 = getUlong(d80);
      verify(ul2_32 == ul1_32);

      const INT64  i1_64  = (INT64)d64;
      const INT64  i2_64  = getInt64(d80);
      verify(i2_64 == i1_64);

      const UINT64 ui1_64 = (UINT64)d64;
      const UINT64 ui2_64 = getUint64(d80);
      verify(ui2_64 == ui1_64);
    }

    TEST_METHOD(Double80TestCast) {
      testAllCast( 0         );
      testAllCast( 1         );
      testAllCast(-1         );
      testAllCast( 1.6       );
      testAllCast(-1.6       );
      testAllCast( 1.3       );
      testAllCast(-1.3       );
      testAllCast( SHRT_MAX  );
      testAllCast( SHRT_MIN  );
      testAllCast(-SHRT_MAX  );
      testAllCast( USHRT_MAX );
      testAllCast( INT_MAX   );
      testAllCast( INT_MIN   );
      testAllCast(-INT_MAX   );
      testAllCast( UINT_MAX  );
      testAllCast(-(double)UINT_MAX  );
      testAllCast( (double)LLONG_MAX );
      testAllCast( (double)LLONG_MIN );
      testAllCast( (double)ULLONG_MAX);
      testAllCast(-(double)ULLONG_MAX);

      Double80 x(LLONG_MAX);
      INT64    l64  = getInt64( x);
      UINT64   ul64 = getUint64(x);
      verify(l64  == LLONG_MAX);
      verify(ul64 == LLONG_MAX);

      x = LLONG_MIN;
      l64  = getInt64( x);
      verify(l64  == LLONG_MIN);

      x = ULLONG_MAX;
      ul64  = getUint64( x);
      verify(ul64 == ULLONG_MAX);
    }

    TEST_METHOD(Double80measureToString) {
      const double startTime = getProcessTime();
      const Double80 stepFactor = 1.012345;
      int count = 0;
      for(Double80 x = Double80::DBL80_MIN; !isNan(x); x *= stepFactor) {
        String s = x.toString();
        count++;
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e3 / count;

      OUTPUT(_T("TimeMeasure on Double80::toString(): count = %d, Time = %.3le msec"), count, timeUsage);
    }

    TEST_METHOD(Double80TestRound) {
      static const double testData[] = {
        1
       ,0.999999
       ,1.499999
       ,1.5
       ,1.99999999999999
      };

      double maxRelativeError = 0;
      for(int i = 0; i < ARRAYSIZE(testData); i++) {
        const double &x = testData[i];
        for(int dec = -21; dec < 21; dec++) {
          double t = x * pow(10, dec);
          for(int j = 0; j < 10; j++, t *= 10) {
            const double error = testRound(t, dec);
            if(error > maxRelativeError) {
              maxRelativeError = error;
            }
          }
        }
      }
      OUTPUT(_T(" Max relative error:%.16le"), maxRelativeError);
    }

    Double80 makeDouble80(UINT64 significand, short exponent, bool positive) {
      BYTE b[10];
      (*(UINT64*)b) = significand;
      (*(USHORT*)(&b[8])) = (USHORT)(((exponent + 0x3fff) & 0x7fff));
      if(!positive) {
        b[9] |= 0x80;
      }
      return Double80(b);
    }

#define SIGNIFICAND(d) ((*((UINT64*)(&(d)))) & 0xffffffffffffffffui64)

    TEST_METHOD(Double80FindEpsilon) {
      for(int i = 1; i < 1024; i *= 2) {
        Double80 a            = 10 * i;
        UINT64   aSignificand = SIGNIFICAND(a);
        int      aExponent    = Double80::getExpo2(a);

        Double80 b            = makeDouble80(aSignificand, aExponent, true);
        UINT64   bSignificand = SIGNIFICAND(b);
        short    bExponent    = Double80::getExpo2(b);
      }

      const UINT64 epsSignificand   = 0x8000000000000001ui64;
      Double80     epsP1            = makeDouble80(epsSignificand, 0, true);
      UINT64       epsP1Significand = SIGNIFICAND(epsP1);
      int          epsP1Exponent    = Double80::getExpo2(epsP1);

      Double80     eps              = epsP1 - Double80::one;
      UINT64       testSignificand  = SIGNIFICAND(eps);
      int          testExponent     = Double80::getExpo2(eps);
      bool         epsPositive      = eps.isPositive();

      OUTPUT(_T("Eps:%s"), eps.toString().cstr());

      Double80 sum = Double80::one + eps;
      verify(sum == epsP1);
      Double80 diff = sum - Double80::one;
      verify(diff == eps);

      char buffer[10];
      memcpy(buffer, &eps, sizeof(buffer));
      hexdump(buffer, sizeof(buffer), stdout);
    }

    TEST_METHOD(Double80FindMax) {
      OUTPUT(_T("DBL80_MAX:%s"), Double80::DBL80_MAX.toString().cstr());
      char buffer[10];
      memcpy(buffer, &Double80::DBL80_MAX, sizeof(buffer));
      hexdump(buffer, sizeof(buffer), stdout);
    }

    TEST_METHOD(Double80TestReadWrite) {
      const char *fileName = "double80.dat";

      const size_t count = 500;
      tofstream out(fileName);
      StreamParameters param(-1);
      size_t i;
      Array<Double80> list;
      for(i = 0; i < count; i++) {
        const Double80 x = randDouble80(-1, 1);
        out << param << x << endl;
        list.add(x);
      }
      out.close();

      tifstream in(fileName);
      const Double80 maxTolerance = 6e-17;
      for(i = 0; i < list.size(); i++) {
        const Double80  &expected = list[i];
        Double80 x;
        in >> x;
        if(in.bad()) {
          OUTPUT(_T("Read Double80 line %d failed"), i);
          verify(false);
        }
        const Double80 relError = getRelativeError(x, expected);
        if(relError > maxTolerance) {
          OUTPUT(_T("Read Double80 at line %d = %s != expected (= %s"), i, toString(x, 18).cstr(), toString(expected, 18).cstr());
          OUTPUT(_T("Relative error:%s"), toString(relError).cstr());
          verify(false);
        }
      }
      in.close();
      UNLINK(fileName);
    }

    TEST_METHOD(Double80TestSinCos) {
      //  USHORT cw = FPU::getControlWord();
      //  printf("FPU ctrlWord:%04x (%s)\n", cw, sprintbin(cw).cstr());

      for(double x = 0; x < 10; x += 0.1) {
        const double d64c1 = cos(x);
        const double d64s1 = sin(x);

        double d64c2 = x, d64s2;
        sincos(d64c2, d64s2);

        verify(fabs(d64c2 - d64c1) < 4e-16);
        verify(fabs(d64s2 - d64s1) < 4e-16);

        /*
        printf("x:%5.2lf, d64c1:%+.16le d64c2:%+.16le diff:%+.10le d64s1:%+.16le d64s2:%+.16le diff:%+.10le\n"
        ,x
        ,d64c1, d64c2, d64c1-d64c2
        ,d64s1, d64s2, d64s1-d64s2
        );
        */

        const Double80 d80   = x;
        const Double80 d80c1 = cos(d80);
        const Double80 d80s1 = sin(d80);

        Double80 d80c2 = d80, d80s2;
        sincos(d80c2, d80s2);

        verify(d80c2 == d80c1);
        verify(d80s2 == d80s1);
      }
      //pause();
    }

    TEST_METHOD(Double80TestBasicOperations) {

      FPU::clearExceptions();

      USHORT cwSave = FPU::getControlWord();

      FPU::setPrecisionMode(FPU_HIGH_PRECISION);
      FPU::enableExceptions(true, FPU_INVALID_OPERATION_EXCEPTION
                                | FPU_DENORMALIZED_EXCEPTION
                                | FPU_DIVIDE_BY_ZERO_EXCEPTION
                                | FPU_OVERFLOW_EXCEPTION
                                | FPU_UNDERFLOW_EXCEPTION);

      USHORT cw1 = FPU::getStatusWord();

      Double80 one        = 1;
      Double80 onePlusEps = one + Double80::DBL80_EPSILON;
      Double80 diff       = onePlusEps - one;

      USHORT cw2 = FPU::getStatusWord();

      verify(diff != 0);

      Double80 onePlusEps2 = one + Double80::DBL80_EPSILON / 2;
      diff = onePlusEps2 - one;
      verify(diff == 0);

      Array<ULONG> hashCodes;

      long             i32max = _I32_MAX;
      Double80         di32   = i32max;
      long             ri32   = getLong(di32);
      long             ri321  = getUlong(di32);

      verify(ri32 == i32max);
      verify(ri321 == i32max);

      hashCodes.add(di32.hashCode());

      long             i32min = _I32_MIN;
      di32 = i32min;
      ri32 = getLong(di32);

      verify(ri32 == i32min);

      hashCodes.add(di32.hashCode());

      ULONG    ul32max = _UI32_MAX;
      Double80 dul32   = ul32max;
      ULONG    rul32   = getUlong(dul32);
      UINT     rui32   = getUint(dul32);

      verify(rul32 == ul32max);
      verify(rui32 == ul32max);

      hashCodes.add(dul32.hashCode());

      INT64          i64max = _I64_MAX;
      Double80       di64   = i64max;
      INT64          ri64   = getInt64(di64);

      verify(ri64 == i64max);

      hashCodes.add(di64.hashCode());

      INT64 i64min = _I64_MIN;
      di64 = i64min;
      ri64 = getUint64(di64);

      verify(ri64 == i64min);

      hashCodes.add(di64.hashCode());

      UINT64   ui64max = _UI64_MAX;
      Double80 dui64   = ui64max;
      UINT64   rui64   = getUint64(dui64);

      verify(rui64 == ui64max);

      Double80 dstr(" 1.23456e123");
      verify(fabs((dstr - 1.23456e123) / dstr) < 1e-15);
      dstr = Double80("+1.23456e123");
      verify(fabs((dstr - 1.23456e123) / dstr) < 1e-15);
      dstr = Double80(" -1.23456e123");
      verify(fabs((dstr + 1.23456e123) / dstr) < 1e-15);
      Double80 dstring(String("-1.23456e123"));
      verify(dstr == dstring);

      hashCodes.add(dui64.hashCode());

      for(size_t i = 0; i < hashCodes.size(); i++) {
        for(size_t j = 0; j < i; j++) {
          verify(hashCodes[i] != hashCodes[j]);
        }
      }

      for(double x = 1e-100; x < 1e100; x *= 1.1) {
        const Double80 d80(x);
        const double   y = getDouble(d80);
        verify(y == x);
      }

      for(double x = -1e-100; x > -1e100; x *= 1.1) {
        const Double80 d80(x);
        const double   y = getDouble(d80);
        verify(y == x);
      }

      Double80 d80;
      for(d80 = -1000; d80 < 1000; ++d80);
      verify(d80 == 1000);
      for(d80 = 1000; d80 > -1000; --d80);
      verify(d80 == -1000);
      for(d80 = -1000; d80 < 1000; d80++);
      verify(d80 == 1000);
      for(d80 = 1000; d80 > -1000; d80--);
      verify(d80 == -1000);
      testFunction("fmod", fmod, fmod, -60, 60, 1.2, 10);
      testFunction("fmod", fmod, fmod, -2.3, 2.7, -1.2, 1);
      testFunction("fmod", fmod, fmod, -1.1e23, 1.1e23, 1.1e-10, 1.45e-6);

      testFunction("sqrt", sqrt, sqrt, 0, 10);
      testFunction("sin", sin, sin, -1, 1);
      testFunction("cos", cos, cos, -1, 1);
      testFunction("tan", tan, tan, -1, 1);
      testFunction("asin", asin, asin, -1, 1);
      testFunction("acos", acos, acos, -1, 1);
      testFunction("atan", atan, atan, -1, 1);
      testFunction("atan2", atan2, atan2, -2.3, 2.7, -1.2, 1);
      testFunction("exp", exp, exp, -1, 1);
      testFunction("log", log, log, 1e-3, 1e3);
      testFunction("log10", log10, log10, 1e-3, 1e3);
      testFunction("log2", log2, log2, 1e-3, 1e3);
      testFunction("pow", pow, pow, 0.1, 2.7e3, -2.1, 2);
      verify(pow(Double80::zero, Double80::one) == Double80::zero);
      verify(pow(Double80::zero, Double80::zero) == Double80::one);
      testFunction("pow10", ::pow10, pow10, -2.4, 12.2);
      testFunction("pow2", ::pow2, pow2, -2.4, 12.2);
      testFunction("root", root, root, 0.1, 2.7e3, -2.1, 2);
      testFunction("fraction", ::fraction, fraction, 1e-3, 1e3);
      testFunction("fraction", ::fraction, fraction, -1e3, -1e-3);
      testFunction("floor", floor, floor, -2.3, 2.7);
      testFunction("ceil", ceil, ceil, -2.3, 2.7);
      testFunction("fabs", fabs, fabs, -1, 1);
      testFunction("Min", ::Min, Min, -2.3, 2.7, -1.2, 1);
      testFunction("Max", ::Max, Max, -2.3, 2.7, -1.2, 1);

      verify(sign(Double80(2)) == 1);
      verify(sign(Double80(-2)) == -1);
      verify(sign(Double80::zero) == 0);

      FPU::enableExceptions(false, FPU_DIVIDE_BY_ZERO_EXCEPTION);

      Double80 zzz = Double80::one / Double80::zero;
      verify(isNan(zzz));
      verify(isInfinity(zzz));
      verify(isPInfinity(zzz));
      verify(!isNInfinity(zzz));

      zzz = (-Double80::one) / Double80::zero;
      verify(isNan(zzz));
      verify(isInfinity(zzz));
      verify(!isPInfinity(zzz));
      verify(isNInfinity(zzz));

      FPU::clearStatusWord();
      FPU::enableExceptions(true, FPU_DIVIDE_BY_ZERO_EXCEPTION);

      FPU::enableExceptions(false, FPU_INVALID_OPERATION_EXCEPTION);

      zzz = sqrt(Double80(-1));
      verify(isNan(zzz));
      verify(!isInfinity(zzz));

      FPU::clearStatusWord();
      FPU::enableExceptions(true, FPU_INVALID_OPERATION_EXCEPTION);

      FPU::setPrecisionMode(FPU_LOW_PRECISION);
      float flt1 = 1 + FLT_EPSILON;
      d80 = flt1;
      float flt2 = getFloat(d80);
      verify(flt2 == flt1);
      double x = 1 + DBL_EPSILON;
      d80 = x;
      double y = getDouble(d80);
      verify(y == x);

      FPU::setPrecisionMode(FPU_NORMAL_PRECISION);
      flt1 = 1 + FLT_EPSILON;
      d80 = flt1;
      flt2 = getFloat(d80);
      verify(flt2 == flt1);
      x = 1 + DBL_EPSILON;
      d80 = x;
      y = getDouble(d80);
      verify(y == x);

      FPU::setPrecisionMode(FPU_HIGH_PRECISION);
      d80 = Double80(1) + Double80::DBL80_EPSILON;
      verify(d80 != 1);
      d80 = Double80(1) + Double80::DBL80_EPSILON / 4;
      verify(d80 == 1);

      verify(FPU::getStackHeight() == 0);
#ifdef IS32BIT
      for(int i = 1; i <= 8; i++) {
        __asm {
          fld x
        }
        verify(FPU::getStackHeight() == i);
      }

      for(int i = 7; i >= 0; i--) {
        __asm {
          fstp x
        }
        verify(FPU::getStackHeight() == i);
      }

      verify(!FPU::stackOverflow());
      verify(!FPU::stackUnderflow());
#endif
      USHORT exp = FPU_INVALID_OPERATION_EXCEPTION
                 | FPU_DENORMALIZED_EXCEPTION
                 | FPU_DIVIDE_BY_ZERO_EXCEPTION
                 | FPU_OVERFLOW_EXCEPTION
                 | FPU_UNDERFLOW_EXCEPTION;

      FPU::enableExceptions(true, exp);
      cw1 = FPU::getControlWord();
      FPU::enableExceptions(false, exp);
      cw2 = FPU::getControlWord();
      USHORT cwDiff = cw1 ^ cw2;
      verify(cwDiff == exp);

      FPU::clearStatusWord();
      verify(FPU::getStatusWord() == 0);

      d80 = sqrt(Double80(2));
      BYTE buffer[10];
      memcpy(buffer, &d80, sizeof(buffer));

      Double80 e80(buffer);
      verify(e80 == d80);

      FPU::setControlWord(cwSave);
    }
  };
}