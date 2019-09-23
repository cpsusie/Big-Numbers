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

	TEST_CLASS(TestComplex)	{
    public:

    TEST_METHOD(BasicOperations) {
//      FPU::init();
#ifdef LONGDOUBLE
      FPU::setPrecisionMode(FPU_HIGH_PRECISION);
#else
//      FPU::setPrecisionMode(FPU_NORMAL_PRECISION);
#endif

      Complex c1, c2;

      setToRandom(c1);
      setToRandom(c2);

      Complex sum = c1 + c2;

      verify(sum.re == c1.re + c2.re && sum.im == c1.im + c2.im);

      Complex add = c1;
      add += c2;

      verify(add == sum);

      Complex dif = c1 - c2;

      verify(dif.re == c1.re - c2.re && dif.im == c1.im - c2.im);

      Complex sub = c1;
      sub -= c2;

      verify(sub == dif);

      Complex save = c1;
      c1 += 3;
      c2 = save + 3;
      verify(c1 == c2);

      c1 -= 3;
      verify(arg(c1 - save) < 1e-14);

      Complex product = c1 * c2;

      Complex c4 = product / c2;

      verify(arg(c1 - c4) < 1e-14);

      Complex prod = c1;

      prod *= c2;

      verify(prod == product);

      prod /= c2;

      verify(prod == c4);

      save = c1;

      c1 *= 3;
      verify(c1 == save * 3);

      c1 /= 3;
      verify(arg(c1 - save) < 1e-14);

      Complex conj = conjugate(c1);

      verify(conj.re == c1.re && conj.im == -c1.im);

      Complex minus = -c1;

      verify(minus.re == -c1.re && minus.im == -c1.im);

      Complex czero;
      verify(czero == 0);
    }

    TEST_METHOD(ComplexFunctions) {
      Complex c1, c2;
#ifdef LONGDOUBLE
      const Real tolerance = 4e-17;
#else
      const Real tolerance = 4e-14;
#endif
      for (int i = 0; i < 100; i++) {
        setToRandom(c1);
        setToRandom(c2);

        const Complex root2 = sqrt(c1);

        verifyAlmostEquals(c1, root2 * root2, tolerance);
        verifyAlmostEquals(exp(c1 + c2), exp(c1)*exp(c2), tolerance);
        verifyAlmostEquals(log(c1*c2), log(c1) + log(c2), tolerance);
        verifyAlmostEquals(log(exp(c1)), c1, tolerance);
        verifyAlmostEquals(pow(c1, c2), exp(log(c1) * c2), tolerance);
        verifyAlmostEquals(root(c1, c2), exp(log(c1) / c2), tolerance);
        verifyAlmostEquals(Complex::one, sqr(sin(c1)) + sqr(cos(c1)), tolerance);
        verifyAlmostEquals(c1, sin(asin(c1)), tolerance);
        verifyAlmostEquals(c1, cos(acos(c1)), tolerance);
        verifyAlmostEquals(tan(c1), sin(c1) / cos(c1), tolerance);
        verifyAlmostEquals(c1, atan(tan(c1)), tolerance);
      }
    }

    TEST_METHOD(TestIdiotRule) {
      redirectDebugLog();
      Complex c1 = Complex(0.59755527779720179,0.333218399476649774);
      const Complex sinc1 = sin(c1);
      const Complex cosc1 = cos(c1);
      const Complex id = sqr(sinc1) + sqr(cosc1);
      const Complex diff = Complex::one - id;
      if (fabs(diff) > 1e-13) {
        OUTPUT(_T("c1:%s, sin(c1):%s, cos(c1)%s, s:%s, diff:%s")
              ,toString(c1,19).cstr()
              ,toString(sinc1,19).cstr(),toString(cosc1,19).cstr()
              ,toString(id).cstr(), toString(diff).cstr()
              );
      }
      verifyAlmostEquals(Complex::one, sqr(sin(c1)) + sqr(cos(c1)), 1e-13);
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

    static CompactComplexArray generateTestArray() {
      CompactComplexArray result;
      result.add( 0);
      result.add( 1);
      result.add(-1);
      result.add(Complex( 1, 1));
      result.add(Complex(-1, 1));

      for(size_t i = 0; i < 15; i++) {
        Complex c;
        setToRandom(c);
        result.add(c);
      }
      return result;
    }

    TEST_METHOD(ComplexIOAllFormats) {
      try {
        const CompactComplexArray a = generateTestArray();

        StreamParametersIterator it               = StreamParameters::getFloatParamIterator(30, NumberInterval<StreamSize>(1, 17));
        const UINT               totalFormatCount = (UINT)it.getMaxIterationCount(), quatil = totalFormatCount/4;
        UINT                     formatCounter    = 0;
        Real maxQ = 0;
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

          for(size_t i = 0; i < a.size(); i++) {
            const Complex &x = a[i];
            costr << param << x << endl;
            wostr << param << x << endl;
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
          for(size_t i = 0; i < a.size(); i++) {
            const Complex &expected = a[i];

            setFormat(cistr, ip);
            setFormat(wistr, ip);
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
