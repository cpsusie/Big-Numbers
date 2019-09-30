#include "stdafx.h"
#include "BigRealGamma.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace TestGamma {		

//#define LIST_DATA

#include <UnitTestTraits.h>

  TEST_CLASS(TestGamma) {
  public:

    TEST_METHOD(GammaD64Positive) {
      double maxError = 0;
      for(double x = 0; x <= 4; x += 0.125) {
        const double  y    = fac(x);
        const BigReal X    = x;
        const BigReal Y    = fac(X, 20);
        const double  refy = getDouble(Y);
        const double  e    = fabs(y - refy);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%+.4lf y:%+23.15le, e:%+23.15le, maxE=%23.15le"), x,y, e, maxError);
#endif // LIST_DATA
        verify(e < 5.5e-12);
      }
    }

    TEST_METHOD(GammaD64Negative) {
      double maxError = 0;
      for(double x = 0; x > -4; x -= 0.125) {
        if(x == floor(x)) continue;
        const double  y    = fac(x);
        const BigReal X    = x;
        const BigReal Y    = fac(X, 20);
        const double  refy = getDouble(Y);
        const double  e    = fabs(y - refy);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%+.4lf y:%+23.15le, e:%+23.15le, maxE=%23.15le"), x,y, e, maxError);
#endif // LIST_DATA
        verify(e < 2.95e-12);

      }
    }

    TEST_METHOD(LnGammaD64) {
      double maxError = 0;
      for(double x = 0.125; x <= 4; x += 0.125) {
        const double y1 = log(gamma(x));
        const double y2 = lnGamma(x);
        const double e  = fabs(y1 - y2);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%+.4lf y1:%+23.15le, y2:%+23.15le, e:%23.15le, maxE=%23.15le")
              ,x, y1, y2, e, maxError);
#endif // LIST_DATA
        verify(e < 4.45e-16);
      }
    }

#define f80(x) ::toString(x,19,27,ios::showpos|ios::scientific).cstr()
#define x80(x) ::toString(x,4,0,  ios::showpos|ios::fixed     ).cstr()
    TEST_METHOD(GammaD80Positive) {
      Double80 maxError = 0;
      for(Double80 x = 0; x <= 4; x += 0.125) {
        const Double80 y    = fac(x);
        const BigReal  X    = x;
        const BigReal  Y    = fac(X, 26);
        const Double80 refy = getDouble80(Y);
        const Double80 e    = fabs(y - refy);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%s y:%s, e:%s, maxE=%s"), x80(x),f80(y), f80(e), f80(maxError));
#endif // LIST_DATA
        verify(e < 4.34e-19);
      }
    }

    TEST_METHOD(GammaD80Negative) {
      Double80 maxError = 0;
      for(Double80 x = 0; x > -4; x -= 0.125) {
        if(x == floor(x)) continue;
        const Double80 y    = fac(x);
        const BigReal  X    = x;
        const BigReal  Y    = fac(X, 26);
        const Double80 refy = getDouble80(Y);
        const Double80 e    = fabs(y - refy);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%s y:%s, e:%s, maxE=%s"), x80(x), f80(y), f80(e), f80(maxError));
#endif // LIST_DATA
        verify(e < 8.68e-19);

      }
    }

    TEST_METHOD(LnGammaD80) {
      Double80 maxError = 0;
      for(Double80 x = 0.125; x <= 4; x += 0.125) {
        const Double80 y1 = log(gamma(x));
        const Double80 y2 = lnGamma(x);
        const Double80 e  = fabs(y1 - y2);
#ifdef LIST_DATA
        if(e > maxError) {
          maxError = e;
        }
        OUTPUT(_T("x:%s y1:%s, y2:%s, e:%s, maxE=%s"), x80(x), f80(y1), f80(y1), f80(e), f80(maxError));
#endif // LIST_DATA
        verify(e < 1.09e-19);
      }
    }
  };
}
