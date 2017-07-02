#include "stdafx.h"
#include <Math/Complex.h>
#include <Math/Polynomial.h>
#include <Tokenizer.h>

ComplexVector root1(double *c) {
  const double A = c[0], B = c[1];
  ComplexVector roots(1);
  roots[0] = -B/A;
  return roots;
}

ComplexVector root2(double *c) {
  const double A = c[0], B = c[1], C = c[2];
  const Complex D = B*B - 4*A*C;

  ComplexVector roots(2);
  roots[0] = (-B + sqrt(D)) / (2*A);
  roots[1] = (-B - sqrt(D)) / (2*A);
  return roots;
}

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

ComplexVector root4(double *c) {
  const double A = c[1]/c[0], B = c[2]/c[0], C = c[3]/c[0], D = c[4]/c[0];
  ComplexVector roots(4);

  if(B == 0 && C == 0 && D == 0) {
    return roots; // 0 is a 4-double root
  }
  const double A2 = A*A;
  const double A3 = A2*A;
  const double p  = B - A2*3/8;
  const double q  = C + A*(A2 - 4*B)/8;
  const double r  = D - A*(3*A3 - 16*A*B + 64*C)/256;
  const double a4 = A/4;
  const double p2 = p*p;

  // We now have the depressed equation y^4 + p*y^2 + q*y + r = 0, where y = x + a4 => x = y - a4

  if(q == 0) { // solve the biquadratic equation y^4 + p*y^2 + r = 0. substitute u = y^2 (=> y = +/-sqrt(u)) giving the equation u^2 + p*u + r = 0.
    const Complex d  = p2 - 4*r;
    const Complex u1 = (-p + sqrt(d))/2;
    const Complex u2 = (-p - sqrt(d))/2;
    const Complex y1 = sqrt(u1);
    const Complex y2 = sqrt(u2);
    roots[0] =  y1 - a4;
    roots[1] = -y1 - a4;
    roots[2] =  y2 - a4;
    roots[3] = -y2 - a4;
  } else {
    double c3[4];
    c3[0] = 1;
    c3[1] = p*5/2;
    c3[2] = 2*p2 - r;
    c3[3] = (p*(p2 - r) - q*q/4)/2;
    ComplexVector rooty = root3(c3);
    Complex y;
    int i;
    for(i = 0; i < 3; i++) {
      y = rooty[i];
      if(-2*y != p) {
        break;
      }
    }
    if(i==3) {
      throwException(_T("Cannot find a root for y which is != %le. roots(y)=%s"), -p/2, rooty.toString().cstr());
    }
    const Complex f1 = sqrt(p+2*y); // != 0 !
    const Complex f2 = 3*p+2*y;
    const Complex f3 = 2*q / f1;
    const Complex f4 = sqrt(-(f2 + f3));
    const Complex f5 = sqrt(-(f2 - f3));
    roots[0] = ( f1 + f4) / 2 - a4;
    roots[1] = ( f1 - f4) / 2 - a4;
    roots[2] = (-f1 + f5) / 2 - a4;
    roots[3] = (-f1 - f5) / 2 - a4;
  }
  return roots;
}

int main(int argc, char **argv) {
  for(;;) {
    _tprintf(_T("Angiv 2-5 koefficenter adskilt a komma:"));
    TCHAR line[200];
    GETS(line);
    double c[5];
    Tokenizer tok(line,_T(", "));
    int i;
    for(i = 0; i < 5 && tok.hasNext(); i++) {
      c[i] = tok.getDouble();
    }

    if(tok.hasNext()) {
      _tprintf(_T("For mange koefficienter angivet. Bruger kun de første 5.\n"));
    } else if(i <= 1) {
      continue;
    }
    if(c[0] == 0) {
      _tprintf(_T("c[0] er 0\n"));
      continue;
    }
    const int degree = i - 1;

    ComplexVector roots;
    switch(degree) {
    case 1:
      roots = root1(c);
      break;

    case 2:
      roots = root2(c);
      break;

    case 3:
      roots = root3(c);
      break;

    case 4:
      roots = root4(c);
      break;
    }

    Vector coef(i);
    for(int j = 0; j < i; j++) {
      coef[j] = c[i-j-1];
    }
    Polynomial poly(coef);
#define FLAGS (ios::scientific|ios::showpos|ios::left)
    _tprintf(_T("roots of f(x)=%s:\n"), poly.toString().cstr());
    for(i = 0; i < (int)roots.getDimension(); i++) {
      const String  xs = toString(roots[i],6,14,FLAGS);
      const Complex y  = poly(roots[i]);
      const String  ys = toString(y,6,14,FLAGS);
      _tprintf(_T("x[%d]:%-32s. f(x[%d])=%-32s. |f(x)|=%le\n"), i+1, xs.cstr(), i+1, ys.cstr(), arg(y));
    }
  }
  return 0;
}
