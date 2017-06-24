#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestPolynomial {		

#include <UnitTestTraits.h>

  Real p1(Real x) {
    return 3 * x + 5;
  }

  Real p2(Real x) {
    return ((3 * x) + 5)* x + 7;
  }

  TEST_CLASS(TestPolynomial) {
  public:
    
    TEST_METHOD(Poly) {
      const Real P_1[] = { 5, 3 };
      const Real P_2[] = { 7, 5, 3};

      const Real P1_1[] = { 3, 5 };
      const Real P1_2[] = { 3, 5, 7};

      for (Real x = -10; x <= 10; x++) {
        verify(p1(x) == poly(x, 1, P_1));
        verify(p2(x) == poly(x, 2, P_2));

        verify(p1(x) == poly1(x, 1, P1_1));
        verify(p2(x) == poly1(x, 2, P1_2));

      }
    }

  };
}