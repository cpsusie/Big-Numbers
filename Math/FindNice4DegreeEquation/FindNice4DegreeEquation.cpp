#include "stdafx.h"
#include <Math/Complex.h>
#include <Math/Polynomial.h>

ComplexVector root3(double *c) {
  const double A = c[1]/c[0], B = c[2]/c[0], C = c[3]/c[0];

  const double A2 = A*A;

  const Complex p  = B - A2/3;
  const Complex q  = C + A*(2*A2-9*B)/27;
  const double  a3 = A/3;

  // We now have the depressed equation y^3 + p*y + q = 0, where y = x + a3 => x = y - a3

  const Complex       d = sqrt(q*q/4 + p*p*p/27);
  const ComplexVector u = roots(q/2 + d, 3);
  ComplexVector       roots(3);

  for(int i = 0; i < 3; i++) {
    const Complex &ui = u[i];
    if(ui == 0) {
      roots[i] = -a3;
    } else {
      roots[i] = p/(3*ui) - ui - a3;
    }
  }
  return roots;
}

bool isNiceValue(const Complex &c) {
  return fabs(c.im) < 1e-10 && (fabs(c.re - round(c.re)) < 1e-10) && (fabs(c.re) > 0.6);
}

bool hasNiceRoot(const ComplexVector &r) {
  for(int i = 0; i < r.getDimension(); i++) {
    if(isNiceValue(r[i])) {
      return true;
    }
  }
  return false;
}

int getNiceRoot(const ComplexVector &r) {
  for(int i = 0; i < r.getDimension(); i++) {
    if(isNiceValue(r[i])) {
      return (int)round(r[i].re);
    }
  }
  return -9999999;
}

int main(int argc, char* argv[]) {
  for(int a = -10; a < 10; a++) {
    if(a == 0) continue;
    for(int b = -10; b < 10; b++) {
      if(b == 0) continue;
      for(int x = -10; x < 10; x++) {
        if(x == 0) continue;
        int c = x*x*x*x + a*x*x - b*x;
        if(c == 0) continue;
        int a3 = -8;
        int a2 = 4 * a;
        int a1 = -8*c;
        int a0 = 4 * a * c + b*b;
        double poly[4];
        poly[0] = a3; poly[1] = a2; poly[2] = a1; poly[3] = a0;
        ComplexVector rootE = root3(poly);
        if(hasNiceRoot(rootE)) {
          _tprintf(_T("ligning: x^4 %+3dx^2 = %dx %+3d. løsning = %d. b=%d\n")
                  , a, b, c, x, getNiceRoot(rootE));
//          printf("a=%+3d, b=%+3d, c=%+6d, x=%+3d, roots:%s\n",a,b,c,x,rootE.toString().cstr());
        }
      }
    }
  }
  return 0;
}
