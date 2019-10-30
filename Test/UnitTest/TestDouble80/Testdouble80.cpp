#include "stdafx.h"
#include <Math.h>
#include <float.h>
#include <limits.h>
#include <Date.h>
#include <Math/Double64.h>
#include <Math/Double80.h>
#include <Math/FPU.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#pragma warning(disable:4101) // unreferenced local variable

using namespace std;

static const double EPS = 3e-14;

namespace TestDouble80 {		

#include <UnitTestTraits.h>

  class LOG : public wostringstream {
  public:
    ~LOG() {
      OUTPUT(_T("%s"), str().c_str());
    }
  };

  static double getRelativeError(double x64, const Double80 &x80) {
    const double relativeError = fabs(x64 - getDouble(x80));
    return (x64 == 0) ? relativeError : relativeError / x64;
  }

  static Double80 getRelativeError(const Double80 &x, const Double80 &expected) {
    const Double80 absError = fabs(x - expected);
    return expected.isZero() ? absError : absError / expected;
  }

  typedef double(  *D641ValFunc)(      double    );
  typedef double(  *D642ValFunc)(      double    ,       double    );
  typedef double(  *D641RefFunc)(const double   &);
  typedef double(  *D642RefFunc)(const double   &, const double   &);

  typedef Double80(*D801ValFunc)(      Double80  );
  typedef Double80(*D802ValFunc)(      Double80  ,       Double80  );
  typedef Double80(*D801RefFunc)(const Double80 &);
  typedef Double80(*D802RefFunc)(const Double80 &, const Double80 &);

  static void testFunction(const String &name, D801ValFunc f80, D641ValFunc f64, double low, double high) {
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
    INFO(_T("%-10s:Max relative error:%.16le"), name.cstr(), maxRelativeError);
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


static void testFunction(const String &name, D802ValFunc f80, D642ValFunc f64, double low1, double high1, double low2, double high2) {
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
    INFO(_T("%-10s:Max relative error:%.16le"), name.cstr(), maxRelativeError);
  }

  static D641RefFunc _F64_1Arg;
  static D642RefFunc _F64_2Arg;
  static D801RefFunc _F80_1Arg;
  static D802RefFunc _F80_2Arg;
  static double f64_1Arg(double x) {
    return _F64_1Arg(x);
  }
  static double f64_2Arg(double x, double y) {
    return _F64_2Arg(x, y);
  }
  static Double80 f80_1Arg(Double80 x) {
    return _F80_1Arg(x);
  }
  static Double80 f80_2Arg(Double80 x, Double80 y) {
    return _F80_2Arg(x, y);
  }

  static void testFunction(const String &name, D801ValFunc f80, D641RefFunc f64, double low, double high) {
    _F64_1Arg = f64;
    testFunction(name, f80, f64_1Arg, low, high);
  }
  static void testFunction(const String &name, D801RefFunc f80, D641ValFunc f64, double low, double high) {
    _F80_1Arg = f80;
    testFunction(name, f80_1Arg, f64, low, high);
  }
  static void testFunction(const String &name, D801RefFunc f80, D641RefFunc f64, double low, double high) {
    _F80_1Arg = f80;
    _F64_1Arg = f64;
    testFunction(name, f80_1Arg, f64_1Arg, low, high);
  }

  static void testFunction(const String &name, D802ValFunc f80, D642RefFunc f64, double low1, double high1, double low2, double high2) {
    _F64_2Arg = f64;
    testFunction(name, f80, f64_2Arg, low1, high1, low2, high2);
  }
  static void testFunction(const String &name, D802RefFunc f80, D642ValFunc f64, double low1, double high1, double low2, double high2) {
    _F80_2Arg = f80;
    testFunction(name, f80_2Arg, f64, low1, high1, low2, high2);
  }
  static void testFunction(const String &name, D802RefFunc f80, D642RefFunc f64, double low1, double high1, double low2, double high2) {
    _F80_2Arg = f80;
    _F64_2Arg = f64;
    testFunction(name, f80_2Arg, f64_2Arg, low1, high1, low2, high2);
  }

#define TESTFUNC(f, ...) testFunction(_T(#f),f,f,__VA_ARGS__)

	TEST_CLASS(TestDouble80) {
    public:

//    TestDouble80() {
//      redirectDebugLog(true);
//      debugLogSetTimePrefix(true,true);
//    }

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

      const FormatFlags dformats[] = { ios::scientific, ios::hexfloat };
      for (size_t f = 0; f < ARRAYSIZE(dformats); f++) {
        const FormatFlags ff = dformats[f];
        for(int i = 1; i >= -1; i -= 2) {
          for(double d64 = 1e-100*i; fabs(d64) < 1e100; d64 *= 1.1) {
            s64 = toString(d64, 13, 21, ff);
            d80 = d64;
            s80 = toString(d80, 13, 21, ff);
            if(s64 != s80) {
              OUTPUT(_T("d64:%23.15le -> \"%s\""), d64, s64.cstr());
              OUTPUT(_T("s80:%s -> \"%s\"\n"), toString(d80, 15, 23, ios::scientific).cstr(), s80.cstr());
            }
          }
        }
      }
    }

    TEST_METHOD(Double80TestStrToD80) {
      Double80 diff;
      TCHAR charBuf[100], *endp;

      const Double80 pi = DBL80_PI;

      const String piStr = d80tot(charBuf, pi);
      diff = pi - wcstod80(piStr.cstr(), &endp);
      verify(diff == 0);
      verify(endp == piStr.cstr() + piStr.length());

      const String maxStr = d80tot(charBuf, DBL80_MAX);
      diff = DBL80_MAX - wcstod80(maxStr.cstr(), &endp);
      verify((diff == 0) && (errno == 0));
      verify(endp == maxStr.cstr() + maxStr.length());

      const String minStr = d80tot(charBuf, DBL80_MIN);
      diff = DBL80_MIN - wcstod80(minStr.cstr(), NULL);
      verify((diff == 0) && (errno == 0));

      const Double80 tmin = numeric_limits<Double80>::denorm_min();
      const String tminStr = d80tot(charBuf, tmin);
      diff = tmin - wcstod80(tminStr.cstr(), NULL);
      verify((diff == 0) && (errno == 0));

      Double80 tmp1 = strtod80("1.18973149535723237e+4932", NULL);
      verify((tmp1 == DBL80_MAX) && (errno == ERANGE));

      tmp1 = strtod80("-1.18973149535723237e+4932", NULL);
      verify((tmp1 == -DBL80_MAX) && (errno == ERANGE));

      for(Double80 d = DBL80_MIN; d != 0; d /= 2) {
        const String dpStr = d80tot(charBuf, d);
        Double80 tmp = wcstod80(dpStr.cstr(), NULL);
        verify((tmp == d) && (errno == 0));
        const Double80 dm = -d;
        const String dmStr = d80tot(charBuf, dm);
        tmp = wcstod80(dmStr.cstr(), NULL);
        verify((tmp == dm) && (errno == 0));
      }

      const Double80 step  = 1.0237432;
      const Double80 start = Double80::pow10(-4927);
      const Double80 end   = Double80::pow10( 4930);
      Double80 maxRelError = 0;
      UINT     nonZeroErrorCount = 0, zeroErrorCount = 0;
      for(Double80 p = start; p < end; p *= step) {
        char str[50];
        Double80 d80 = randDouble80(0,p);
        d80toa(str,d80);
        Double80 d80a = strtod80(str, NULL);
        const Double80 err = getRelativeError(d80a, d80);
        if(err.isZero()) {
          zeroErrorCount++;
        } else {
          nonZeroErrorCount++;
          if(err > maxRelError) {
            maxRelError = err;
            if(err > 3e-19) {
              TCHAR errstr[50], diffstr[50];
              String s = str;
              OUTPUT(_T("Fejl for d80=%s: Relative error:%s, diff=%s"), s.cstr(), d80tot(errstr, err), d80tot(diffstr,d80a-d80));
              verify(false);
            }
          }
        }
      }
      TCHAR maxstr[50];
      INFO(_T("%s:Max relative Error:%s"), __TFUNCTION__, d80tot(maxstr, maxRelError));
      const UINT totalCompares = zeroErrorCount + nonZeroErrorCount;
      INFO(_T("%s:Total cconversions:%s, Non-zero errors:%.02lf%%")
            , __TFUNCTION__
            ,format1000(totalCompares).cstr()
            ,PERCENT(nonZeroErrorCount, totalCompares));
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
      for(Double80 x = DBL80_MIN; isfinite(x); x *= stepFactor) {
        const int d = Double80::getExpo10(x);
        count++;
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e3 / count;

      INFO(_T("TimeMeasure on Double80::getExpo10(): count = %d, Time = %.3le msec"), count, timeUsage);
    }

    TEST_METHOD(Double80TestPow2) {
      const int minExpo2 = getExpo2(DBL80_MIN);
      const int maxExpo2 = getExpo2(DBL80_MAX);
      const double startTime = getProcessTime();
      int count = 0;
      for(int p = minExpo2; p <= maxExpo2; p++) {
        count++;
        const Double80 d1 = Double80::pow2(p);
        const Double80 d2 = exp2(Double80(p));
        verify(d1 == d2);
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e3 / count;

      INFO(_T("TimeMeasure on Double80::pow2(): count = %d, Time = %.3le msec"), count, timeUsage);
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
      TCHAR tmp[50];
      TMEMSET(tmp, 0, ARRAYSIZE(tmp));
      for(Double80 x = DBL80_MIN; isfinite(x); x *= stepFactor) {
        d80tot(tmp, x);
        count++;
      }
      const double timeUsage = (getProcessTime() - startTime) / 1e3 / count;

      INFO(_T("TimeMeasure on Double80::toString(): count = %d, Time = %.3le msec"), count, timeUsage);
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
      INFO(_T("%s:Max relative error:%.16le"), __TFUNCTION__, maxRelativeError);
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

#define SIGNIFICAND(d) (*((UINT64*)(&(d))))

    TEST_METHOD(Double80FindEpsilon) {
      Double80 p = 1e-100;
      const Double80 f = 2.123456;
      for(int i = 1; i < 1024; i++, p *= f) {
        const Double80 a            = p;
        const UINT64   aSignificand = SIGNIFICAND(a);
        const int      aExponent    = getExpo2(a);

        const Double80 b            = makeDouble80(aSignificand, aExponent, true);
        const UINT64   bSignificand = getSignificand(b);
        const int      bExponent    = getExponent(b);
        verify(bSignificand == aSignificand);
        verify(aExponent    == bExponent - 0x3fff);
      }

      const UINT64 epsSignificand   = 0x8000000000000001ui64;
      Double80     epsP1            = makeDouble80(epsSignificand, 0, true);
      UINT64       epsP1Significand = SIGNIFICAND(epsP1);
      int          epsP1Exponent    = getExpo2(epsP1);

      Double80     eps              = epsP1 - Double80::_1;
      UINT64       testSignificand  = SIGNIFICAND(eps);
      int          testExponent     = getExpo2(eps);
      bool         epsPositive      = eps.isPositive();
      TCHAR        tmpStr[50];
      INFO(_T("Eps:%s"), d80tot(tmpStr, eps));

      const Double80 sum = Double80::_1 + eps;
      verify(sum == epsP1);
      const Double80 diff = sum - Double80::_1;
      verify(diff == eps);

      char buffer[10];
      memcpy(buffer, &eps, sizeof(buffer));
      hexdump(buffer, sizeof(buffer), stdout);
    }

    TEST_METHOD(Double80FindMax) {
      TCHAR tmpStr[50];
      OUTPUT(_T("DBL80_MAX:%s"), d80tot(tmpStr, DBL80_MAX));
      char buffer[10];
      memcpy(buffer, &DBL80_MAX, sizeof(buffer));
      hexdump(buffer, sizeof(buffer), stdout);
    }

#undef min
#undef max

    static double unitRand64(RandomGenerator &rnd) {
      return randDouble(-1, 1, rnd);
    }
    static Double80 unitRand80(RandomGenerator &rnd) {
      return randDouble80(-1, 1, rnd);
    }

#undef endl

    template<class DType> DType _testReadWrite(DType      (*unitRand)(RandomGenerator &rnd)
                                             ,const DType &maxTolerance
                                             ,const TCHAR *dtypeName
                                             ) {
      const String fileName = getTestFileName(String(__TFUNCTION__) + String(dtypeName));
      JavaRandom rnd(1234);

//      debugLog(_T("%s\n%s\n"), __TFUNCTION__, FPU::getState().toString().cstr());

      const size_t count = 500;
      StreamParameters param(numeric_limits<DType>::max_digits10);
      CompactArray<DType> list(count);

      list.add(numeric_limits<DType>::lowest()        );
      list.add( numeric_limits<DType>::max()          );
      list.add(numeric_limits<DType>::epsilon()       );
      for(DType x = numeric_limits<DType>::min();;x /= 2) {
        list.add(x);
        if(x == 0) {
          break;
        }
      }

      list.add(-numeric_limits<DType>::infinity()     );
      list.add( numeric_limits<DType>::infinity()     );
      list.add( numeric_limits<DType>::quiet_NaN()    );
      list.add( numeric_limits<DType>::signaling_NaN());

      for(size_t i = 0; i < count; i++) {
        const DType x = unitRand(rnd);
        list.add(x);
      }
      ofstream out(fileName.cstr());
      for(size_t i = 0; i < list.size(); i++) {
        out << param << list[i] << endl;
      }
      out.close();

      DType detectedMaxRelError = 0;
      ifstream in(fileName.cstr());
      for(size_t i = 0; i < list.size(); i++) {
        const DType &expected = list[i];
        DType data;
        in >> CharManip<DType> >> data;
        if(in.bad() || in.fail()) {
          OUTPUT(_T("Read %s line %zu failed"), __TFUNCTION__, i);
          verify(false);
        }
        if(isfinite(data)) {
          const DType relError = getRelativeError(data, expected);
          if(relError > detectedMaxRelError) {
            detectedMaxRelError = relError;
          }
          if(relError > maxTolerance) {
            OUTPUT(_T("%s:Read %s at line %d = %s != expected (=%s"), __TFUNCTION__, dtypeName, i, toString(data, 18).cstr(), toString(expected, 18).cstr());
            OUTPUT(_T("Relative error:%s"), toString(relError).cstr());
            verify(false);
          }
        } else if(isPInfinity(data)) {
          verify(isPInfinity(expected));
        } else if(isNInfinity(data)) {
          verify(isNInfinity(expected));
        } else if(isnan(data)) {
          verify(isnan(expected));
        } else {
          throwException(_T("%s:Unknown classification for a[%zu]:%s"), __TFUNCTION__, i, toString(data).cstr());
        }
      }
      in.close();
      INFO(_T("%s:Detected max. relative error:%s"), __TFUNCTION__, toString(detectedMaxRelError).cstr());
      return detectedMaxRelError;
    }

    TEST_METHOD(TestReadWrite) {
      verify(_testReadWrite<Double80>( unitRand80, 2e-19, _T("Double80")) < 1.3e-19);
      verify(_testReadWrite<double  >( unitRand64, 0    , _T("double")  ) ==      0);
    }

    TEST_METHOD(Double80TestSinCos) {
      //  USHORT cw = FPU::getControlWord();
      //  printf("FPU ctrlWord:%04x (%s)\n", cw, sprintbin(cw).cstr());

//      debugLog(_T("%s\n%s\n"), __TFUNCTION__, FPU::getState().toString().cstr());

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
      int swSize   = sizeof(FPUStatusWord );
      int cwSize   = sizeof(FPUControlWord);
      int tgSize   = sizeof(FPUTagWord    );
      verify(sizeof(FPUEnvironment) == 28 );
      verify(sizeof(FPUState      ) == 108);

//      debugLog(_T("%s\n%s\n"), __TFUNCTION__, FPU::getState().toString().cstr());
      FPU::clearExceptions();

      const FPUControlWord ctrlSave = FPU::getControlWord();

      FPUControlWord ctrlWord = ctrlSave;
      FPU::setControlWord(ctrlWord.setPrecisionMode(FPU_HIGH_PRECISION)
                                  .adjustExceptionMask(FPU_INVALID_OPERATION_EXCEPTION
                                                     | FPU_DENORMALIZED_EXCEPTION
                                                     | FPU_DIVIDE_BY_ZERO_EXCEPTION
                                                     | FPU_OVERFLOW_EXCEPTION
                                                     | FPU_UNDERFLOW_EXCEPTION
                                                     , 0));

      const FPUStatusWord sw1 = FPU::getStatusWord();

      Double80 one        = 1;
      Double80 onePlusEps = one + DBL80_EPSILON;
      Double80 diff       = onePlusEps - one;

      const FPUStatusWord sw2 = FPU::getStatusWord();

      verify(diff != 0);

      Double80 onePlusEps2 = one + DBL80_EPSILON / 2;
      diff = onePlusEps2 - one;
      verify(diff == 0);

      CompactArray<ULONG> hashCodes;

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

      ULONG          ul32max  = _UI32_MAX;
      Double80       dul32    = ul32max;
      ULONG          rul32    = getUlong(dul32);
      UINT           rui32    = getUint(dul32);

      verify(rul32 == ul32max);
      verify(rui32 == ul32max);

      hashCodes.add(dul32.hashCode());

      INT64          i64max   = _I64_MAX;
      Double80       di64     = i64max;
      INT64          ri64     = getInt64(di64);

      verify(ri64 == i64max);

      hashCodes.add(di64.hashCode());

      INT64           i64min  = _I64_MIN;
      di64 = i64min;
      ri64 = getUint64(di64);

      verify(ri64 == i64min);

      hashCodes.add(di64.hashCode());

      UINT64          ui64max = _UI64_MAX;
      Double80        dui64   = ui64max;
      UINT64          rui64   = getUint64(dui64);

      verify(rui64 == ui64max);

      Double80 dstr = strtod80(" 1.23456e123", NULL);
      verify(fabs((dstr - 1.23456e123) / dstr) < 1e-15);
      dstr = strtod80("+1.23456e123", NULL);
      verify(fabs((dstr - 1.23456e123) / dstr) < 1e-15);
      dstr = strtod80(" -1.23456e123", NULL);
      verify(fabs((dstr + 1.23456e123) / dstr) < 1e-15);
      Double80 dstring = wcstod80(L"-1.23456e123", NULL);
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
      TESTFUNC(fmod     , -60    , 60     ,  1.2    , 10     );
      TESTFUNC(fmod     , -2.3   , 2.7    , -1.2    , 1      );
      TESTFUNC(fmod     , -1.1e23, 1.1e23 , 1.1e-10 , 1.45e-6);

      TESTFUNC(sqrt     ,  0     ,  10    );
      TESTFUNC(sin      , -1     ,   1    );
      TESTFUNC(cos      , -1     ,   1    );
      TESTFUNC(tan      , -1     ,   1    );
      TESTFUNC(asin     , -1     ,   1    );
      TESTFUNC(acos     , -1     ,   1    );
      TESTFUNC(atan     , -1     ,   1    );
      TESTFUNC(atan2    , -2.3   , 2.7    , -1.2, 1);
      TESTFUNC(exp      , -1     ,   1    );
      TESTFUNC(exp10    , -2.4   ,  12.2  );
      TESTFUNC(exp2     , -2.4   ,  12.2  );
      TESTFUNC(log      , 1e-3   ,   1e3  );
      TESTFUNC(log10    , 1e-3   ,   1e3  );
      TESTFUNC(log2     , 1e-3   ,   1e3  );
      TESTFUNC(pow      , 0.1    ,   2.7e3, -2.1, 2);
      verify(pow(Double80::_0, Double80::_1)  == Double80::_0);
      verify(pow(Double80::_0, Double80::_0) == Double80::_1 );
      TESTFUNC(root     , 0.1    ,   2.7e3, -2.1, 2);
      TESTFUNC(fraction , 1e-3   ,   1e3  );
      TESTFUNC(fraction ,-1e3    ,  -1e-3 );
      TESTFUNC(floor    ,-2.3    ,   2.7  );
      TESTFUNC(ceil     ,-2.3    ,   2.7  );
      TESTFUNC(fabs     ,-1      ,   1    );
      testFunction("dmin", dmin<Double80>    , dmin<double>, -2.3, 2.7, -1.2, 1);
      testFunction("dmax", dmax<Double80>    , dmax<double>, -2.3, 2.7, -1.2, 1);

      verify(sign(Double80( 2)  ) ==  1);
      verify(sign(Double80(-2)  ) == -1);
      verify(sign(Double80::_0  ) ==  0);

      FPU::adjustExceptionMask(0,FPU_DIVIDE_BY_ZERO_EXCEPTION);

/*
      Double80 zzz = Double80::_0 / Double80::_0;
      verify(isnan(       zzz));
      verify(!isinf(      zzz));
      verify(!isPInfinity(zzz));
      verify(!isNInfinity(zzz));
*/
      Double80 zzz = Double80::_1 / Double80::_0;
      verify(!isnan(      zzz));
      verify(isinf(       zzz));
      verify(isPInfinity( zzz));
      verify(!isNInfinity(zzz));

      zzz = (-Double80::_1) / Double80::_0;
      verify(!isnan(      zzz));
      verify(isinf(       zzz));
      verify(!isPInfinity(zzz));
      verify(isNInfinity( zzz));

      verify( isnan(      numeric_limits<Double80>::quiet_NaN()));
      verify(!isinf(      numeric_limits<Double80>::quiet_NaN()));
      verify(!isPInfinity(numeric_limits<Double80>::quiet_NaN()));
      verify(!isNInfinity(numeric_limits<Double80>::quiet_NaN()));

      verify( isnan(      numeric_limits<Double80>::signaling_NaN()));
      verify(!isinf(      numeric_limits<Double80>::signaling_NaN()));
      verify(!isPInfinity(numeric_limits<Double80>::signaling_NaN()));
      verify(!isNInfinity(numeric_limits<Double80>::signaling_NaN()));

      verify( isnan(      DBL80_NAN ));
      verify(!isinf(      DBL80_NAN ));
      verify(!isPInfinity(DBL80_NAN ));
      verify(!isNInfinity(DBL80_NAN ));

      verify(!isnan(      DBL80_PINF));
      verify( isinf(      DBL80_PINF));
      verify( isPInfinity(DBL80_PINF));
      verify(!isNInfinity(DBL80_PINF));

      verify(!isnan(      DBL80_NINF));
      verify( isinf(      DBL80_NINF));
      verify(!isPInfinity(DBL80_NINF));
      verify( isNInfinity(DBL80_NINF));

      FPU::clearStatusWord();
      FPU::adjustExceptionMask(FPU_DIVIDE_BY_ZERO_EXCEPTION,FPU_INVALID_OPERATION_EXCEPTION);

      zzz = sqrt(Double80(-1));
      verify(isnan(zzz));
      verify(!isinf(zzz));

      FPU::clearStatusWord();
      FPU::adjustExceptionMask(FPU_INVALID_OPERATION_EXCEPTION,0);

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
      d80 = Double80(1) + DBL80_EPSILON;
      verify(d80 != 1);
      d80 = Double80(1) + DBL80_EPSILON / 4;
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
      const USHORT excep = FPU_INVALID_OPERATION_EXCEPTION
                         | FPU_DENORMALIZED_EXCEPTION
                         | FPU_DIVIDE_BY_ZERO_EXCEPTION
                         | FPU_OVERFLOW_EXCEPTION
                         | FPU_UNDERFLOW_EXCEPTION;

      const FPUControlWord cwa = FPU::adjustExceptionMask(excep,0);
      const FPUControlWord cwb = FPU::getControlWord();
      const FPUControlWord cwc = FPU::adjustExceptionMask(0, excep);
      const FPUControlWord cwd = FPU::getControlWord();
      const USHORT         cwDiff = cwb ^ cwd;
      verify(cwDiff == excep);

      FPU::clearStatusWord();
      verify(FPU::getStatusWord().m_data == 0);

      d80 = sqrt(Double80(2));
      BYTE buffer[10];
      memcpy(buffer, &d80, sizeof(buffer));

      const Double80 e80(buffer);
      verify(e80 == d80);

      FPU::restoreControlWord(ctrlSave);
    }

    template<class T> class Pair {
    private:
      static inline String toString(bool b) {
        return boolToStr(b);
      }
      const String m_fltTypeName, m_op2TypeName;
      const String m_operatorName, m_fltValue, m_d80Value, m_op2Value;
      const T      m_fltResult, m_d80Result;

    public:

      Pair(const String &fltTypeName
          ,const String &op2TypeName
          ,const String &operatorName
          ,const String &fltv, const String &d80v, const String &op2Value
          ,T fltResult, T d80Result)
        : m_fltTypeName( fltTypeName)
        , m_op2TypeName( op2TypeName)
        , m_operatorName(operatorName)
        , m_fltValue(fltv), m_d80Value(d80v), m_op2Value(op2Value)
        , m_fltResult(fltResult), m_d80Result(d80Result)
      {
      }
      inline bool isValid() const {
        return m_fltResult == m_d80Result;
      }
      String toString() const {
        return format(_T("(%-6s)%-6s %-2s (%-6s)%-20s:%-6s   (%s)%-9s %-2s (%-6s)%-20s:%-6s")
                     ,m_fltTypeName.cstr(),m_fltValue.cstr(), m_operatorName.cstr(), m_op2TypeName.cstr(), m_op2Value.cstr(), toString(m_fltResult).cstr()
                     ,_T("Double80")      ,m_d80Value.cstr(), m_operatorName.cstr(), m_op2TypeName.cstr(), m_op2Value.cstr(), toString(m_d80Result).cstr()
                     );
      }
    };
#define BoolPair Pair<bool>
#define IntPair  Pair<int>

    template<class FLTTYPE, class OP2TYPE> void testCompare(FLTTYPE flt, OP2TYPE op2, const String &fltTypeName, const String &op2TypeName) {
      const Double80 d80 = flt;

#define CompareOp2(name, op, operand) BoolPair name(fltTypeName, op2TypeName, #op              \
                                              ,toString(flt), toString(d80), toString(operand) \
                                              ,flt op operand, d80 op operand)
#define CompareOpI(name, op) CompareOp2(name, op,        op2)
#define CompareOpZ(name, op) CompareOp2(name, op, (OP2TYPE)0)

#define verifyPair(name)  if(!name.isValid()) OUTPUT(_T("%s"), name.toString().cstr())

//      IntPair classify(fpclassify(flt), fpclassify(d80));
      CompareOpI(eq  , == );
      CompareOpI(neq , != );
      CompareOpI(ge  , >= );
      CompareOpI(le  , <= );
      CompareOpI(gt  , >  );
      CompareOpI(lt  , >  );
      CompareOpZ(zero, == );
      CompareOpZ(pos , >  );
      CompareOpZ(neg , <  );

//      verifyPair(classify);
      verifyPair(eq      );
      verifyPair(neq     );
      verifyPair(ge      );
      verifyPair(le      );
      verifyPair(gt      );
      verifyPair(lt      );
      verifyPair(zero    );
      verifyPair(pos     );
      verifyPair(neg     );
    }

    template<class FLTTYPE, class OP2TYPE> void testCompareAllUndef(OP2TYPE op2, const String &fltTypeName, const String &op2TypeName) {
      testCompare( numeric_limits<FLTTYPE>::infinity()     , op2, fltTypeName, op2TypeName);
      testCompare(-numeric_limits<FLTTYPE>::infinity()     , op2, fltTypeName, op2TypeName);
      testCompare( numeric_limits<FLTTYPE>::signaling_NaN(), op2, fltTypeName, op2TypeName);
    }

    template<class OP2TYPE> void testAllFloatTypes(OP2TYPE op2, const String &op2TypeName) {
      testCompareAllUndef<float,  OP2TYPE>(op2, "float" , op2TypeName);
      testCompareAllUndef<double, OP2TYPE>(op2, "double", op2TypeName);
    }

    template<class OP2TYPE> void testValues(const String &op2TypeName) {
      testAllFloatTypes((OP2TYPE) 0, op2TypeName);
      testAllFloatTypes((OP2TYPE) 1, op2TypeName);
      testAllFloatTypes((OP2TYPE)-1, op2TypeName);
      testAllFloatTypes(numeric_limits<OP2TYPE>::min(), op2TypeName);
      testAllFloatTypes(numeric_limits<OP2TYPE>::max(), op2TypeName);
      if(numeric_limits<OP2TYPE>::has_infinity) {
        testAllFloatTypes(numeric_limits<OP2TYPE>::infinity() , op2TypeName);
#pragma warning(disable:4146) // unary minus operator applied to unsigned type.....not importatnt here...coz unsigned long has no infinity
        testAllFloatTypes(-(OP2TYPE)numeric_limits<OP2TYPE>::infinity(), op2TypeName);
      }
      if(numeric_limits<OP2TYPE>::has_quiet_NaN) {
        testAllFloatTypes(numeric_limits<OP2TYPE>::quiet_NaN(), op2TypeName);
      }
      if(numeric_limits<OP2TYPE>::has_signaling_NaN) {
        testAllFloatTypes(numeric_limits<OP2TYPE>::signaling_NaN(), op2TypeName);
      }
    }

    TEST_METHOD(Double80CompareUndef) {
      testValues<SHORT >("SHORT" );
      testValues<USHORT>("USHORT");
      testValues<INT   >("INT"   );
      testValues<UINT  >("UINT"  );
      testValues<INT64 >("INT64" );
      testValues<UINT64>("UINT64");
      testValues<float>( "float" );
      testValues<double>("double");
    }
 };
}