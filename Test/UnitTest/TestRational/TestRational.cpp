#include "stdafx.h"
#include "CppUnitTest.h"
#include <math.h>
#include <Random.h>
#include <Math/MathLib.h>
#include <Math/Rational.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define VERIFYOP(op, maxError) {                                                                     \
  const Rational rBinResult = r1 op r2; const double dBinResult = d1 op d2;                          \
  const double   dFromR     = getDouble(rBinResult);                                                 \
  const double   error      = fabs(dFromR - dBinResult);                                             \
  if(error > maxError) {                                                                             \
    throwException(_T("%s %s %s = %s = %23.16le. %23.16le %s %23.16le = %23.16le. Error:%le > %le")  \
                  ,r1.toString().cstr()                                                              \
                  ,#op                                                                               \
                  ,r2.toString().cstr()                                                              \
                  ,rBinResult.toString().cstr(), dFromR                                              \
                  ,d1, #op, d2, dBinResult                                                           \
                  ,error, maxError                                                                   \
                  );                                                                                 \
  }                                                                                                  \
}

namespace TestRational {		

#include <UnitTestTraits.h>

	TEST_CLASS(TestRational) {
    public:

    TEST_METHOD(RationalTest) {
      randomize();

      //  double maxTotalError = 0;
      for (int i = 0; i < 100000; i++) {
        /*
        if(i % 100 == 99) {
        printf("i:%10d. maxTotalError:%le\r", i, maxTotalError);
        }
        */
        const int nm1 = randInt(-1000, 1000);
        const int dn1 = randInt(1, 1000000);
        const int nm2 = randInt(-1000, 1000);
        const int dn2 = randInt(1, 1000000);

        const Rational r1(nm1, dn1);
        const Rational r2(nm2, dn2);

        const double d1 = getDouble(r1);
        const double d2 = getDouble(r2);

        const bool rgt = r1 >  r2, dgt = d1 >  d2;
        const bool rge = r1 >= r2, dge = d1 >= d2;
        const bool rlt = r1 <  r2, dlt = d1 <  d2;
        const bool rle = r1 <= r2, dle = d1 <= d2;
        const bool req = r1 == r2, deq = d1 == d2;
        const bool rne = r1 != r2, dne = d1 != d2;

        verify(rgt == dgt);
        verify(rge == dge);
        verify(rlt == dlt);
        verify(rle == dle);
        verify(req == deq);
        verify(rne == dne);

        VERIFYOP(+, 1e-13)
        VERIFYOP(-, 1e-13)
        VERIFYOP(*, 1e-14)

        if (!r2.isZero()) {
          VERIFYOP(/ , 3e-9)
        }

        const int      expo = r1.isZero() ? randInt(0, 4) : randInt(-3, 3);
        const Rational r1Pe = pow(r1, expo);
        const Real     d1Pe = mypow(d1, expo);
        Real           error = fabs(getReal(r1Pe) - d1Pe);
        if (d1Pe != 0) error /= d1Pe;
        verify(error < 1e-15);


        const Rational rfd(d1);
        const double   dfr = getDouble(rfd);
        error = fabs(dfr - d1);
        verify(error < 1e-13);

      }
    }
  };
}