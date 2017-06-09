#include "stdafx.h"
#include <Math/Complex.h>
#include <Math/Polynomial.h>

static bool isNiceValue(const Complex &c) {
  return fabs(c.im) < 1e-10 && (fabs(c.re - round(c.re)) < 1e-10) && (fabs(c.re) > 0.6);
}

int main(int argc, char* argv[]) {
  for(int p = -20; p < 20; p++) {
    if(p == 0) continue;
    for(int x = -15; x < 15; x++) {
      if(x == 0) continue;
      const int q = -x*x*x - p*x;
      if(q == 0) continue;
      const double t = q*q/4 + p*p*p/27;

      if(t > -0.5) continue;

      const double sqt = sqrt(-t);
      if(!isNiceValue(sqt)) continue;

      const Complex isqt = sqrt(Complex(t));
      const Complex u1 = root(q/2 + isqt, 3);
      const Complex u2 = root(q/2 - isqt, 3);
      const Complex v1 = p/(3*u1);
      const Complex v2 = p/(3*u2);
      const Complex x1 = v1 - u1;
      const Complex x2 = v2 - u2;

      if(!isNiceValue(x1) || !isNiceValue(x2)) continue;
      Complex check1 = x1*x1*x1+p*x1+q;
      Complex check2 = x2*x2*x2+p*x2+q;

      if((fabs(check1) > 1e-10) || (fabs(check2) > 1e-10)) {
        printf("FEJL---");
      }
      _tprintf(_T("ligning: x^3 %+3dx %+3d = 0,  ((q/2)^2 + (p/3)^3) = %4.0lf, løsninger:x1=%s, x2=%s, u1=%s, u2=%s,v1=%s,v2=%s\n")
            ,p, q, t
            ,toString(x1).cstr(), toString(x2).cstr()
            ,toString(u1).cstr(), toString(u2).cstr()
            ,toString(v1).cstr(), toString(v2).cstr()
            );
    }
  }
  return 0;
}
