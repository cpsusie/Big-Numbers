#include "stdafx.h"
#include <math.h>
#include <Random.h>
#include <MatrixTemplate.h>
#include <CompactLineArray.h>
#include <CompactHashSet.h>
#include <StrStream.h>
#include <Math/MathLib.h>
#include <Math/Double80.h>
#include <Math/Number.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;


static const StreamSize typePrecision[] = {
  numeric_limits<float   >::max_digits10
 ,numeric_limits<double  >::max_digits10
 ,numeric_limits<Double80>::max_digits10
 ,0
};

#define Fstr(x,prec) toString(x, prec,0,ios::scientific).cstr()
#define F6( x) Fstr(x,6)
#define F19(x) Fstr(x,19)
#define Fn(n)  Fstr(n, typePrecision[n.getType()])
#define Fd80(d80) Fstr(d80, numeric_limits<Double80>::max_digits10)

#define SHOWOPERROR(f,op, maxError)                                   \
  f(_T("%s %s %s = %s, %s %s %s = %s. Error:%s > %s")                 \
    , Fn(  n1), _T(#op), Fn(  n2), Fn(  nBinResult)                   \
    , Fd80(d1), _T(#op), Fd80(d2), Fd80(dBinResult)                   \
    , Fd80(relError), Fd80(maxError)                                  \
   );

#define VERIFYOP(op, maxError)                                        \
  nBinResult = (n1) op (n2);                                          \
  dBinResult = (d1) op (d2);                                          \
  relError   = getDouble80(getRelativeError(nBinResult, dBinResult)); \
  if(relError > maxError) {                                           \
    maxError = relError;                                              \
/*    SHOWOPERROR(OUTPUT,op, maxError)  */                            \
  }


class OperandResultTypeCount {
public:
  UINT m_counter;
  UINT m_resultType[4];
  OperandResultTypeCount()
    : m_counter(0)
  {
    memset(m_resultType, 0, sizeof(m_resultType));
  }
  OperandResultTypeCount(int v)
    : m_counter(0)
  {
    memset(m_resultType, 0, sizeof(m_resultType));
  }
};

String toString(const OperandResultTypeCount &c) {
  return format(_T("%5u [%5u,%5u,%5u,%5u]")
               ,c.m_counter
               ,c.m_resultType[0]
               ,c.m_resultType[1]
               ,c.m_resultType[2]
               ,c.m_resultType[3]
  );
}

wostream &operator<<(wostream &out, const OperandResultTypeCount &c) {
  out.width(31);
  out << toString(c);
  return out;
}

namespace TestNumber {		

  static const NumberType numberTypes[] = {
    NUMBERTYPE_FLOAT
   ,NUMBERTYPE_DOUBLE
   ,NUMBERTYPE_DOUBLE80
   ,NUMBERTYPE_RATIONAL
  };

  #include <UnitTestTraits.h>

#define RANDOM_NUMBERTYPE ((NumberType)-1)
  Number randNumber(NumberType type = RANDOM_NUMBERTYPE, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    if(type == RANDOM_NUMBERTYPE) {
      type = numberTypes[randInt(ARRAYSIZE(numberTypes), rnd)];
    }
    switch(type) {
    case NUMBERTYPE_FLOAT   : return randFloat(   rnd);
    case NUMBERTYPE_DOUBLE  : return randDouble(  rnd);
    case NUMBERTYPE_DOUBLE80: return randDouble80(rnd);
    case NUMBERTYPE_RATIONAL: return randRational(rnd);
    default                 : return randFloat(   rnd);
    }
  }

  typedef Array<Number> NumberArray;

  static NumberArray generateTestArray(size_t count = 1000, NumberType type = RANDOM_NUMBERTYPE, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator) {
    NumberArray result;
    for(size_t i = 0; i < count; i++) {
      result.add(randNumber(type, rnd));
    }
    return result;
  }

  static Number getRelativeError(const Number &x, const Number &expected) {
    const Number absError = fabs(x - expected);
    return expected.isZero() ? absError : absError / expected;
  }

  class ErrorMatrix : public MatrixTemplate<Double80> {
  private:
    const TCHAR m_opName;
    bool        m_ok;
    double      m_maxError[4]; // diag-elements max-value
    int         m_errorRow, m_errorCol;
    void setError(int r, int c) {
      m_errorRow = r; m_errorCol = c;
      m_ok = false;
    }
  public:
    ErrorMatrix(char op, double max00, double max11, double max22, double max33)
      : MatrixTemplate<Double80>(4, 4)
      , m_opName(op)
      , m_ok(true)
    {
      m_maxError[0] = max00;
      m_maxError[1] = max11;
      m_maxError[2] = max22;
      m_maxError[3] = max33;
    }
    bool checkOk();
    String toString(StreamSize prec=5) const {
      return format(_T("Operator(%c)\n"), m_opName)
           + format(_T("maxTolerance(%le,%le,%le,%le\n")
                   ,m_maxError[0], m_maxError[1], m_maxError[2], m_maxError[3])
           + (m_ok?EMPTYSTRING:format(_T("Error in row %d, col %d\n"), m_errorRow, m_errorCol))
           + (TowstringStream(prec, prec + 7, ios::scientific) << *this).str().c_str();
    }
  };

  bool ErrorMatrix::checkOk() {
    for(int r = 0; r < 4; r++) {
      for(int c = 0; c < 4; c++) {
        const Double80 &e = (*this)(r, c);
        if(r != c) {
          if(e!=0) {
            setError(r, c);
            return false;
          }
        } else if(e > m_maxError[r]) {
          setError(r, c);
          return false;
        }
      }
    }
    return m_ok;
  }

  class CountMatrix : public MatrixTemplate<OperandResultTypeCount> {
  private:
    const TCHAR m_opName;
  public:
    CountMatrix(char op)
      : m_opName(op)
      , MatrixTemplate<OperandResultTypeCount>(4, 4)
    {
    }
    String toString() const {
      return format(_T("Operator(%c)\n"), m_opName)
           + (TowstringStream(31, ios::dec) << *this).str().c_str();
    }
  };

  TEST_CLASS(TestNumber) {
    public:

    TEST_METHOD(NumberTestBasicOperations) {
      JavaRandom rnd(45);
      try {
        ErrorMatrix MmaxErrorAdd('+',5e-6  ,1.2e-16,0,1.07e-19);
        ErrorMatrix MmaxErrorSub('-',4e-7  ,0      ,0,1.2e-16);
        ErrorMatrix MmaxErrorMul('*',1.4e-6,1.1e-16,0,1.3e-19);
        ErrorMatrix MmaxErrorDiv('/',5.6e-7,1.1e-16,0,1.1e-19);
  //      CountMatrix combiCountAdd('+'), combiCountSub('-'), combiCountMul('*'), combiCountDiv('/');
        for(int i = 0; i < 10000; i++) {
          /*
          if(i % 100 == 99) {
          printf("i:%10d. maxTotalError:%le\r", i, maxTotalError);
          }
          */
          Number n1(randNumber(RANDOM_NUMBERTYPE, rnd)), n2(randNumber(RANDOM_NUMBERTYPE, rnd));
  /*
          OperandResultTypeCount &opCountAdd = combiCountAdd(n1.getType(), n2.getType());
          OperandResultTypeCount &opCountSub = combiCountSub(n1.getType(), n2.getType());
          OperandResultTypeCount &opCountMul = combiCountMul(n1.getType(), n2.getType());
          OperandResultTypeCount &opCountDiv = combiCountDiv(n1.getType(), n2.getType());
  */
          Double80 &maxErrorAdd = MmaxErrorAdd(n1.getType(), n2.getType());
          Double80 &maxErrorSub = MmaxErrorSub(n1.getType(), n2.getType());
          Double80 &maxErrorMul = MmaxErrorMul(n1.getType(), n2.getType());
          Double80 &maxErrorDiv = MmaxErrorDiv(n1.getType(), n2.getType());

          const Double80 d1 = getDouble80(n1);
          const Double80 d2 = getDouble80(n2);

          const bool ngt = n1 >  n2, dgt = d1 >  d2;
          const bool nge = n1 >= n2, dge = d1 >= d2;
          const bool nlt = n1 <  n2, dlt = d1 <  d2;
          const bool nle = n1 <= n2, dle = d1 <= d2;
          const bool neq = n1 == n2, deq = d1 == d2;
          const bool nne = n1 != n2, dne = d1 != d2;

          verify(ngt == dgt);
          verify(nge == dge);
          verify(nlt == dlt);
          verify(nle == dle);
          verify(neq == deq);
          verify(nne == dne);

          Number   nBinResult;
          Double80 dBinResult, relError;

          VERIFYOP(+, maxErrorAdd) // 1e-13
  //        opCountAdd.m_resultType[nBinResult.getType()]++;
  //        opCountAdd.m_counter++;

          VERIFYOP(-, maxErrorSub)
  //        opCountSub.m_resultType[nBinResult.getType()]++;
  //        opCountSub.m_counter++;

          VERIFYOP(*, maxErrorMul) // 1e-14
  //        opCountMul.m_resultType[nBinResult.getType()]++;
  //        opCountMul.m_counter++;

          if(!n2.isZero()) {
            VERIFYOP(/ , maxErrorDiv) // 3e-9
  //          opCountDiv.m_resultType[nBinResult.getType()]++;
  //          opCountDiv.m_counter++;
          }

          const int      expo = n1.isZero() ? randInt(0, 4) : randInt(-3, 3);
          const Number   n1Pe = pow(  n1, expo);
          const Double80 d1Pe = mypow(d1, expo);
          Double80       nerror = fabs(getDouble80(n1Pe) - d1Pe);
          if(d1Pe != 0) {
            nerror /= d1Pe;
          }
          verify(nerror < 1e-15);

          const Number   nfd(d1);
          const Double80 dfn = getDouble80(nfd);
          nerror = fabs(dfn - d1);
          verify(nerror < 1e-13);
        }

        if(!MmaxErrorAdd.checkOk()) {
          OUTPUT(_T("%s"), MmaxErrorAdd.toString().cstr()); verify(false);
        }
        if(!MmaxErrorSub.checkOk()) {
          OUTPUT(_T("%s"), MmaxErrorSub.toString().cstr()); verify(false);
        }
        if(!MmaxErrorMul.checkOk()) {
          OUTPUT(_T("%s"), MmaxErrorMul.toString().cstr()); verify(false);
        }
        if(!MmaxErrorDiv.checkOk()) {
          OUTPUT(_T("%s"), MmaxErrorDiv.toString().cstr()); verify(false);
        }


        INFO(_T("%s"), MmaxErrorAdd.toString().cstr());
        INFO(_T("%s"), MmaxErrorSub.toString().cstr());
        INFO(_T("%s"), MmaxErrorMul.toString().cstr());
        INFO(_T("%s"), MmaxErrorDiv.toString().cstr());
        INFO(_T("%s"), combiCountAdd.toString().cstr());
        INFO(_T("%s"), combiCountSub.toString().cstr());
        INFO(_T("%s"), combiCountMul.toString().cstr());
        INFO(_T("%s"), combiCountDiv.toString().cstr());

        } catch(Exception e) {
          OUTPUT(_T("%s"), e.what());
          verify(false);
        }
    }

    static Number testNumberToFromStr(const Number &n) {
      TCHAR charBuf[300], *endp;
      const String   str = numtot(charBuf, n);
      const Number   n1  = _tcstonum(str.cstr(), &endp);

      verify(errno == 0);
      verify(endp == str.cstr() + str.length());
      return getRelativeError(n1, n);
    }

    TEST_METHOD(NumberTestToFromString) {
      JavaRandom rnd(56);
      const double maxRelErrorArray[] = {
        5e-10 // NUMBERTYPE_FLOAT
       ,5e-18 // NUMBERTYPE_DOUBLE
       ,2e-19 // NUMBERTYPE_DOUBLE80
       ,0     // NUMBERTYPE_RATIONAL
      };
      for(size_t i = 0; i < ARRAYSIZE(numberTypes); i++) {
        const NumberType  type = numberTypes[i];
        const NumberArray a    = generateTestArray(10000, type);
        Number detectedMaxRelError = 0;
        const Number maxRelError = maxRelErrorArray[i];
        for (size_t i = 0; i < a.size(); i++) {
          const Number &n = a[i];
          const Number relError = testNumberToFromStr(n);
          if(relError > detectedMaxRelError) {
            detectedMaxRelError = relError;
          }
          if(relError > maxRelError) {
            OUTPUT(_T("%s(%s): %s")
                  ,__TFUNCTION__, Number::getTypeName(type).cstr()
                  ,toString(n, 18).cstr()
                  );
            OUTPUT(_T("Relative error:%s > max (= %s)")
                  ,toString(relError).cstr()
                  ,toString(maxRelError).cstr()
                  );
            verify(false);
          }
        }
        INFO(_T("%s(%s):Detected max relative error:%s")
            , __TFUNCTION__, Number::getTypeName(type).cstr()
            , toString(detectedMaxRelError).cstr());
      }
    }


    void testReadWrite(NumberType type, const Number &maxTolerance, int precision, RandomGenerator &rnd) {
      const size_t count = 10000;
      StreamParameters param(precision);
      const NumberArray list = generateTestArray(count, type, rnd);

      stringstream  coutstr;
      wstringstream woutstr;
      setFormat(coutstr, param);
      setFormat(woutstr, param);
      for(size_t i = 0; i < list.size(); i++) {
        coutstr << list[i] << endl;
        woutstr << list[i] << endl;
      }

      string  cstr = coutstr.str();
      wstring wstr = woutstr.str();
      verify(String(cstr.c_str()) == String(wstr.c_str()));

      Number detectedMaxRelError = 0;
      stringstream  cinstr(cstr);
      wstringstream winstr(wstr);
      for(size_t i = 0; i < list.size(); i++) {
        const Number &expected = list[i];
        Number data;
        cinstr >> data;
        if(cinstr.bad() || cinstr.fail()) {
          OUTPUT(_T("Read %s line %zu failed"), __TFUNCTION__, i);
          verify(false);
        }
        if(isfinite(data)) {
          const Number relError = getRelativeError(data, expected);
          if(relError > detectedMaxRelError) {
            detectedMaxRelError = relError;
          }
          if(relError > maxTolerance) {
            OUTPUT(_T("%s(%s):Line %zu = %s != expected (=%s)")
                  ,__TFUNCTION__,Number::getTypeName(type).cstr()
                  ,i
                  ,toString(data, 18).cstr()
                  ,toString(expected, 18).cstr());
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
      INFO(_T("%s(%s):Detected max relative error:%s")
          ,__TFUNCTION__,Number::getTypeName(type).cstr()
          , toString(detectedMaxRelError).cstr());
    }

    TEST_METHOD(TestReadWrite) {
      JavaRandom rnd(56);
      testReadWrite(NUMBERTYPE_FLOAT   , 5e-9 , numeric_limits<float   >::max_digits10, rnd);
      testReadWrite(NUMBERTYPE_DOUBLE  , 5e-17, numeric_limits<double  >::max_digits10, rnd);
      testReadWrite(NUMBERTYPE_DOUBLE80, 2e-19, numeric_limits<Double80>::max_digits10, rnd);
      testReadWrite(NUMBERTYPE_RATIONAL, 0    , 0                                     , rnd);
    }

    template<class T> void verifyNumberNanTypes(const T &f) {
      const bool fisNan  = isnan(      f);
      const bool fisInf  = isinf(      f);
      const bool fisPInf = isPInfinity(f);
      const bool fisNInf = isNInfinity(f);
      const Number n(f);

      verify(isnan(      n)  == fisNan );
      verify(isinf(      n)  == fisInf );
      verify(isPInfinity(n)  == fisPInf);
      verify(isNInfinity(n)  == fisNInf);

      const float    f1  = getFloat(   n);
      const double   d1  = getDouble(  n);
      const Double80 d80 = getDouble80(n);
      const Rational rat = getRational(n);

      verify(isnan(      f1 ) == fisNan );
      verify(isinf(      f1 ) == fisInf );
      verify(isPInfinity(f1 ) == fisPInf);
      verify(isNInfinity(f1 ) == fisNInf);

      verify(isnan(      d1 ) == fisNan );
      verify(isinf(      d1 ) == fisInf );
      verify(isPInfinity(d1 ) == fisPInf);
      verify(isNInfinity(d1 ) == fisNInf);

      verify(isnan(      d80) == fisNan );
      verify(isinf(      d80) == fisInf );
      verify(isPInfinity(d80) == fisPInf);
      verify(isNInfinity(d80) == fisNInf);

      verify(isnan(      rat) == fisNan );
      verify(isinf(      rat) == fisInf );
      verify(isPInfinity(rat) == fisPInf);
      verify(isNInfinity(rat) == fisNInf);
    }

#pragma warning(disable : 4723) // Potential divide by 0
#pragma warning(disable : 4756) // overflow in constant arithmetic

    template<class T> void numberNanTest(const T m) {
      T f = (T)sqrt(-1);
      verifyNumberNanTypes(f);

      f = m;
      f *= 2;
      verifyNumberNanTypes(f);

      f = m;
      f *= -2;
      verifyNumberNanTypes(f);

      f = 1;
      T g = 0;
      f /= g;
      verifyNumberNanTypes(f);

      f = -1;
      f /= g;
      verifyNumberNanTypes(f);

      f =  0;
      f /= g;
      verifyNumberNanTypes(f);
    }

    TEST_METHOD(NumberNaN) {
      try {
        numberNanTest(FLT_MAX  );
        numberNanTest(DBL_MAX  );
        numberNanTest(DBL80_MAX);
        numberNanTest(RAT_MAX  );
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }

    class NumPowerKey {
    public:
      Rational m_base;
      int      m_e;
      NumPowerKey() {
      }
      NumPowerKey(const Rational &b, int e) : m_base(b), m_e(e) {
      }
      inline ULONG hashCode() const {
        return m_base.hashCode() + 31 * m_e;
      }
      inline bool operator==(const NumPowerKey &rhs) const {
        return (m_e == rhs.m_e) && (m_base == rhs.m_base);
      }
    };

    typedef CompactHashSet<NumPowerKey> NumPowerHashSet;

#define setRelError(e) {                    \
  const Number relError = e;                \
  if(relError > detectedMaxRelError) {      \
    detectedMaxRelError = relError;         \
  }                                         \
  if(relError > maxTolerance) {             \
    OUTPUT(_T("Relative error:%s > max=%s") \
          ,toString(relError).cstr()        \
          ,toString(maxTolerance).cstr());  \
    verify(false);                          \
  }                                         \
}

    TEST_METHOD(NumberPowers) {
      try {
        Number detectedMaxRelError = 0;
        Number maxTolerance = 1.19e-18;
        NumPowerHashSet done;
        for(INT64 den = 1; den <= 16; den++) {
          for(INT64 num = -16; num <= 16; num++) {
            for(int e = -10; e <= 10; e++) {
              if(e == 0) continue;
              const Rational base(num, den);
              NumPowerKey key(base, e);
              if(done.contains(key)) continue;
              done.add(key);
              const Number   n1b(base);
              const Number   n1bpe = pow(n1b,e);
              const Number   n2e(Rational(2, e));
              const Number   n3e(Rational(3, e));
              const Number   n1result2 = pow(n1bpe, n2e); // = root((n1b^e)^2,e) (= n1b^2)
              const Number   n1result3 = pow(n1bpe, n3e); // = root((n1b^e)^3,e) (= n1b^3)
              bool nanResult = false;
              if(n1b.isZero() && (e<=0)) {
                verify(isnan(n1bpe));
                verify(isnan(n1result2));
                verify(isnan(n1result3));
                nanResult = true;
              } else {
                verify(n1result2.getType() == NUMBERTYPE_RATIONAL);
                verify(n1result3.getType() == NUMBERTYPE_RATIONAL);
                const Number   n1bsqr = n1b    * n1b;
                const Number   n1bcub = n1bsqr * n1b;
                verify(n1result2 == n1bsqr);
                if(n1b.isPositive() || isOdd(e)) {
                  verify(n1result3 == n1bcub);
                } else { // (n1b<0) && e even => n1b^3<0 && root(n1b^e)^3,e)>0
                  verify(-n1result3 == n1bcub);
                }
              }
              const Number n2bpe = mypow(getDouble80(n1b), e);
              if(nanResult) {
                verify(!isfinite(n2bpe));
                const Number n2result2 = pow(n2bpe, n2e);
                verify(!isfinite(n2result2));
              } else {
                setRelError(getRelativeError(n1bpe, n2bpe));
                if(n2bpe.getType() != NUMBERTYPE_RATIONAL) {
                  const Number n2result2 = pow(n2bpe, n2e);
                  setRelError(getRelativeError(n2result2, n1result2));
                  const Number n2result3 = pow(n2bpe, n3e);
                  setRelError(getRelativeError(n2result3, n1result3));
                }
              }
            }
          }
        }
        INFO(_T("%s:Detected max relative error:%s")
            ,__TFUNCTION__
            ,toString(detectedMaxRelError).cstr());

      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
  };
}
