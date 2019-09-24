#include "pch.h"
#include <Math/Complex.h>

const Complex Complex::i    = Complex(0,1);
const Complex Complex::zero = Complex(0,0);
const Complex Complex::one  = Complex(1,0);

#define EATWHITE() while(_istspace(*s)) s++

Complex exp(const Complex &c) {
  Complex result;
  result.re = c.im;
  sincos(result.re,result.im);
  const Real l = exp(c.re);
  result.re *= l; result.im *= l;
  return result;
}

Complex sqrt(const Complex &c) {
  Polar p(c);
  Complex result;
  result.re = p.theta / 2;
  sincos(result.re,result.im);
  p.r = sqrt(p.r);
  result.re *= p.r; result.im *= p.r;
  return result;
}

inline Complex reciprocal(const Complex &c) {
  const Real d = sqr(c.re) + sqr(c.im);
  return Complex(c.re / d, -c.im / d);
}

inline Complex multiplyI(const Complex &c) {
  return Complex(-c.im, c.re);
}

inline Complex divideI(const Complex &c) {
  return Complex(c.im, -c.re);
}

Complex sin(const Complex &c) {
  const Complex c1 = exp(multiplyI(c));
  return divideI(c1 - reciprocal(c1)) / 2.0;
}

Complex cos(const Complex &c) {
  const Complex c1 = exp(multiplyI(c));
  return Complex(c1 + reciprocal(c1)) / 2.0;
}

Complex tan(const Complex &c) {
  const Complex c1 = exp(multiplyI(c));
  const Complex c2 = reciprocal(c1);
  return divideI(c1-c2)/(c1+c2);
}

class PolarAB {
public:
  Real A,B;
  PolarAB(const Complex &c);
};

PolarAB::PolarAB(const Complex &c) {
  const Real b  = sqr(c.im);
  const Real t1 = sqrt(sqr(c.re+1) + b) / 2;
  const Real t2 = sqrt(sqr(c.re-1) + b) / 2;
  A = t1 + t2;
  B = t1 - t2;

//  cout << " " << A << endl;

  if(B > 1) {
    B = 1;
  } else if(B < -1) {
    B = -1;
  }
}

Complex asin(const Complex &c) {
  const PolarAB T(c);
  const Real re = asin(T.B);
  const Real im = log(T.A + sqrt((sqr(T.A)-1)));
  return Complex(re, im);
}

Complex acos(const Complex &c) {
  const PolarAB T(c);
  const Real re = acos(T.B);
  const Real im = -log(T.A + sqrt((sqr(T.A)-1)));
  return Complex(re, im);
}

Complex atan(const Complex &c) {
  const Real &a = c.re;
  const Real &b = c.im;
  const Real a2 = sqr(a);
  return Complex(
           atan2(2.0*a, 1.0 - a2 - b*b) / 2
          ,log((a2 + sqr(b+1)) / (a2 + sqr(b-1))) / 4
         );
}

void setToRandom(Complex &c) {
  setToRandom(c.re);
  setToRandom(c.im);
}
