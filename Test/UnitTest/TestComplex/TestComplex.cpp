#include "stdafx.h"
#include "CppUnitTest.h"
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

    TEST_METHOD(ComplexTest) {
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

      for (int i = 0; i < 100; i++) {
        setToRandom(c1);
        setToRandom(c2);

        Complex root2 = sqrt(c1);

        verifyAlmostEquals(c1, root2 * root2, 1e-13);
        verifyAlmostEquals(exp(c1 + c2), exp(c1)*exp(c2), 1e-13);
        verifyAlmostEquals(log(c1*c2), log(c1) + log(c2), 1e-13);
        verifyAlmostEquals(log(exp(c1)), c1, 1e-13);
        verifyAlmostEquals(pow(c1, c2), exp(log(c1) * c2), 1e-13);
        verifyAlmostEquals(root(c1, c2), exp(log(c1) / c2), 1e-13);
        verifyAlmostEquals(Complex::one, sqr(sin(c1)) + sqr(cos(c1)), 1e-13);
        verifyAlmostEquals(c1, sin(asin(c1)), 1e-13);
        verifyAlmostEquals(c1, cos(acos(c1)), 1e-13);
        verifyAlmostEquals(tan(c1), sin(c1) / cos(c1), 1e-13);
        verifyAlmostEquals(c1, atan(tan(c1)), 1e-13);
      }

      Complex czero;
      verify(czero == 0);

      String str = toString(Complex(0));
      verify(str == "0");

      str = toString(Complex(1, 1));
      verify(str == "(1,1)");

      str = toString(Complex(1));
      verify(str == "1");

      str = toString(Complex(1, 1));
      verify(str == "(1,1)");

      const char *fileName = "complex.tmp";

      tofstream out(fileName);
      out << Complex(1, 1) << _T(",") << Complex(1) << _T(",") << 1 << _T(",") << _T("(1,1") << endl;
      out.close();

      tifstream in(fileName);
      TCHAR ch;
      in >> c1 >> ch >> c2;

      verify(c1 == Complex(1, 1));
      verify(c2 == 1);

      in >> ch >> c1;
      verify(c1 == 1);

      in >> ch >> c1;
      verify(in.bad());

      in.close();

      UNLINK(fileName);
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
  };
}