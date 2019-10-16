#include "stdafx.h"
#include <CompactLineArray.h>
#include <StrStream.h>
#include <Math/MathLib.h>
#include <Math/Double80.h>
#include <Math/FPU.h>
#include <Math/Complex.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestComplex {		

#include <UnitTestTraits.h>

  static Real getRelativeError(const Complex &c, const Complex &expected) {
    const Real error = fabs(c - expected);
    const Real d     = fabs(expected);
    return (d == 0) ? error : (error / d);
  }

#define verifyRelError(expected, v, maxRelError)               \
{ const Complex _v       = v;                                  \
  const Real    relError = getRelativeError(_v, expected);     \
  if(relError > detectedMaxRelError) {                         \
    detectedMaxRelError = relError;                            \
  }                                                            \
  if(relError > maxRelError) {                                 \
    OUTPUT(_T("%s=%s. Expected:%s. rel.Error=%s > max (=%s)")  \
          ,_T(#v), toString(_v,17).cstr()                      \
          ,toString(expected,17).cstr()                        \
          ,toString(relError   , 10).cstr()                    \
          ,toString(maxRelError,10).cstr()                     \
          );                                                   \
    verify(false);                                             \
  }                                                            \
}

	TEST_CLASS(TestComplex)	{
    public:

    TEST_METHOD(BasicOperations) {
//      FPU::init();
#ifdef LONGDOUBLE
        FPU::setPrecisionMode(FPU_HIGH_PRECISION);
#else
//      FPU::setPrecisionMode(FPU_NORMAL_PRECISION);
#endif

      JavaRandom rnd(16);
      for(int i = 0; i < 100; i++) {
        Complex c1, c2;

        setToRandom(c1, &rnd);
        setToRandom(c2, &rnd);

        const Complex sum = c1 + c2;
        verify(sum.re == c1.re + c2.re && sum.im == c1.im + c2.im);
        Complex add = c1; add += c2;
        verify(add == sum);

        const Complex dif = c1 - c2;
        verify(dif.re == c1.re - c2.re && dif.im == c1.im - c2.im);
        Complex sub = c1; sub -= c2;
        verify(sub == dif);

        Complex product = c1 * c2;
        Complex prod = c1; prod *= c2;
        verify(prod == product);

        Complex quotient = c1 / c2;
        Complex quot = c1; quot /= c2;
        verify(quotient == quot);

        Complex saveC1 = c1;
        Complex s1     = saveC1 + 3;
        Complex s2     = saveC1; s2 += 3;
        verify(s1 == s2);

        saveC1 = c1;
        s1     = saveC1 - 3;
        s2     = saveC1; s2 -= 3;
        verify(s1 == s2);

        saveC1 = c1;
        s1 = saveC1 * 3;
        s2 = saveC1; s2 *= 3;
        verify(s1 == s2);

        saveC1 = c1;
        s1 = saveC1 / 3;
        s2 = saveC1; s2 /= 3;
        verify(s1 == s2);

        Complex conj = conjugate(c1);
        verify(conj.re == c1.re && conj.im == -c1.im);

        Complex minus = -c1;
        verify(minus.re == -c1.re && minus.im == -c1.im);

        Complex z;
        verify(z == 0);
        z = Complex::_0;
        verify((z.re == 0) && (z.im == 0));

        c1 = Complex::_1;
        verify((c1.re == 1) && (c1.im == 0));

        c1 = Complex::i;
        verify((c1.re == 0) && (c1.im == 1));
      }
    }

    TEST_METHOD(ComplexFunctions) {
      Complex c1, c2;
      JavaRandom rnd(20);
      Real detectedMaxRelError = 0;
      for(int i = 0; i < 100; i++) {
        setToRandom(c1, &rnd);
        setToRandom(c2, &rnd);

        const Complex root2 = sqrt(c1);
        verifyRelError(c1          , root2   * root2            , 3.8e-16);
        verifyRelError(exp(c1 + c2), exp(c1) * exp(c2)          , 4e-16  );
        verifyRelError(log(c1 * c2), log(c1) + log(c2)          , 3.2e-16);
        verifyRelError(log(exp(c1)), c1                         , 5e-15  );
        verifyRelError(pow(c1, c2) , exp(log(c1) * c2)          , 0      );
        verifyRelError(root(c1, c2), exp(log(c1) / c2)          , 0      );
        verifyRelError(Complex::_1 , sqr(sin(c1)) + sqr(cos(c1)), 6.8e-16);
        verifyRelError(c1          , sin(asin(c1))              , 2.6e-12);
        verifyRelError(c1          , cos(acos(c1))              , 2.6e-12);
        verifyRelError(tan(c1)     , sin(c1) / cos(c1)          , 0      );
        verifyRelError(c1          , atan(tan(c1))              , 4.9e-15);
      }
      INFO(_T("%s:detected max.relError=%s"), __TFUNCTION__,toString(detectedMaxRelError, 17).cstr());
    }

    TEST_METHOD(TestRoots) {
      JavaRandom rnd(20);
#ifdef LONGDOUBLE
      const Real tolerance = 4e-17;
#else
      const Real tolerance = 2e-14;
#endif

      for(int r = 1; r <= 15; r++) {
        Real detectedMaxRelError = 0;
        for(int i = 0; i < 10; i++) {
          Complex c;
          setToRandom(c, &rnd);
          const ComplexVector rootVector = roots(c, r);
          verify(rootVector.getDimension() == r);
          for(int j = 0; j < r; j++) {
            const Complex &ri = rootVector[j];
            const Complex pi = pow(ri, r);
            verifyRelError(c, pi, tolerance);
          }
        }
        INFO(_T("%s:detected max.relError=%s"), __TFUNCTION__,toString(detectedMaxRelError, 17).cstr());
      }
    }

    TEST_METHOD(TestPowNegativeReal) {
      const Complex c1 = -2, c2(0.5, 0.4);
      Complex r, p;
      r = root(c1, c2);
      p = pow( r , c2);

      Complex p2 = pow(r, c2*c2);

      p = pow( c1, c2);
      r = root( p, c2);
    }

    TEST_METHOD(TestIdiotRule) {
      redirectDebugLog();
      Real detectedMaxRelError = 0;
      Complex c1 = Complex(0.59755527779720179,0.333218399476649774);
      const Complex sinc1 = sin(c1);
      const Complex cosc1 = cos(c1);
      const Complex id = sqr(sinc1) + sqr(cosc1);
      const Complex diff = Complex::_1 - id;
      if(fabs(diff) > 1e-13) {
        OUTPUT(_T("c1:%s, sin(c1):%s, cos(c1)%s, s:%s, diff:%s")
              ,toString(c1,19).cstr()
              ,toString(sinc1,19).cstr(),toString(cosc1,19).cstr()
              ,toString(id).cstr(), toString(diff).cstr()
              );
      }
      verifyRelError(Complex::_1, sqr(sin(c1)) + sqr(cos(c1)), 1e-13);
    }

    TEST_METHOD(ComplexIO) {
      String str = toString(Complex(0));
      verify(str == "0");

      str = toString(Complex(1, 1));
      verify(str == "(1,1)");

      str = toString(Complex(1));
      verify(str == "1");

      str = toString(Complex(1, 1));
      verify(str == "(1,1)");

      const String fileName = getTestFileName(__TFUNCTION__, _T("txt"));

      tofstream out(fileName.cstr());
      out << Complex(1, 1) << _T(",") << Complex(1) << _T(",") << 1 << _T(",") << _T("(1,1") << endl;
      out.close();

      tifstream in(fileName.cstr());
      Complex c1, c2;

      TCHAR ch;
      in >> c1 >> ch >> c2;

      verify(c1 == Complex(1, 1));
      verify(c2 == 1);

      in >> ch >> c1;
      verify(c1 == 1);

      in >> ch >> c1;
      verify(in.fail());

      in.close();
    }

    typedef CompactArray<Complex> CompactComplexArray;

    static CompactComplexArray generateTestArray(RandomGenerator *rnd = _standardRandomGenerator) {
      CompactComplexArray result;
      result.add( 0);
      result.add( 1);
      result.add(-1);
      result.add(Complex( 1, 1));
      result.add(Complex(-1, 1));

      for(size_t i = 0; i < 15; i++) {
        Complex c;
        setToRandom(c, rnd);
        result.add(c);
      }
      return result;
    }

    TEST_METHOD(ComplexIOAllFormats) {
      try {
        JavaRandom                rnd(31234);
        const CompactComplexArray a                = generateTestArray(&rnd);
        StreamParametersIterator  it               = StreamParameters::getFloatParamIterator(30, NumberInterval<StreamSize>(1, 17),0, ITERATOR_FLOATFORMATMASK&~ios::internal);
        const UINT                totalFormatCount = (UINT)it.getMaxIterationCount(), quatil = totalFormatCount/4;
        UINT                      formatCounter    = 0;
        while(it.hasNext()) {
          const StreamParameters &param = it.next();
          if(++formatCounter % quatil == 0) {
            OUTPUT(_T("%s progress:%.2lf%%"), __TFUNCTION__, PERCENT(formatCounter, totalFormatCount));
          }
          if((param.flags()&ios::adjustfield) == ios::internal) {
            continue;
          }

          ostringstream  costr;
          wostringstream wostr;

//          OUTPUT(_T("formatCounter:%d format:%s"), formatCounter, param.toString().cstr());
          setFormat(costr, param);
          setFormat(wostr, param);
          const StreamSize w = param.width();
          for(size_t i = 0; i < a.size(); i++) {
            const Complex &x = a[i];
            costr.width(w);
            wostr.width(w);
            costr << x << endl;
            wostr << x << endl;
          }
          const string  cstr = costr.str();
          const wstring wstr = wostr.str();
          verify(String(cstr.c_str()) == String(wstr.c_str()));

          CompactLineArray lineArray(wstr);
          verify((StreamSize)lineArray.minLength() >= param.width());

          istringstream  cistr(cstr);
          wistringstream wistr(wstr);

          const Real tolerance = sqrt(0.5) * pow(10.0, -param.precision());
          StreamParameters ip(param);
          ip.flags(param.flags() | ios::skipws);
          setFormat(cistr, ip);
          setFormat(wistr, ip);
          for(size_t i = 0; i < a.size(); i++) {
            const Complex &expected = a[i];

            if(!iswspace(ip.fill())) {
              skipspace(cistr);
              skipfill(cistr);
              skipspace(wistr);
              skipfill(wistr);
            }
            Complex cx, wx;

            cistr >> cx;
            wistr >> wx;
            verify(cx == wx);
            verify(fabs(cx - expected) <= tolerance);
          }
        }
      } catch(Exception e) {
        OUTPUT(_T("Exception:%s"), e.what());
        verify(false);
      }
    }
  };
}
