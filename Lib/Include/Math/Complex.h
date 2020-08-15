#pragma once

#include <MatrixTemplate.h>
#include <MathUtil.h>
#include "MathLib.h"
#include "Point2D.h"

class Complex {
public:
  Real re, im;
#if defined(LONGDOUBLE)
  inline Complex(double _re, double _im = 0) : re(_re), im(_im) {
  }
#endif

  inline Complex() : re(0), im(0) {
  }
  inline Complex(int _re) : re(_re), im(0) {
  }
  inline Complex(UINT _re) : re(_re), im(0) {
  }
  inline Complex(const Real &_re, const Real &_im = 0) : re(_re), im(_im) {
  }

  inline void save(ByteOutputStream &s) const {
    s.putBytes((BYTE*)this, sizeof(Complex));
  }
  inline void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(Complex));
  }

  friend inline Packer &operator<<(Packer &p, const Complex &c) {
    return p << c.re << c.im;
  }
  friend inline Packer &operator>>(Packer &p, Complex &c) {
    return p >> c.re >> c.im;
  }

  static const Complex _0;
  static const Complex _05; // 0.5
  static const Complex _1;
  static const Complex  i;
};

class Polar {
private:
  inline void init(Real x, Real y) {
    r = sqrt(x*x + y*y);
    theta = atan2(y, x);
  }
public:
  Real r, theta;
  inline Polar() {
    r = theta = 0;
  }
  inline Polar(const Complex &c) {
    init(c.re, c.im);
  }
  Polar(const Point2D &p) {
    init(p.x, p.y);
  }
  inline operator Complex() const {
    return Complex(r*cos(theta), r*sin(theta));
  }
  operator Point2D() const {
    return Point2D(r*cos(theta), r*sin(theta));
  }
  inline String toString(StreamSize dec = 1, bool rad = false) const {
    return format(_T("(%s, %s)"), ::toString(r, dec).cstr(), ::toString(rad ? theta : RAD2GRAD(theta), dec).cstr());
  }
};

inline Complex  operator+(const Complex &lts, const Complex &rhs) {
  return Complex(lts.re + rhs.re,lts.im + rhs.im);
}
inline Complex  operator-( const Complex &lts, const Complex &rhs) {
  return Complex(lts.re - rhs.re,lts.im - rhs.im);
}
inline Complex  operator*(const Complex &lts, const Complex &rhs) {
  return Complex(lts.re * rhs.re - lts.im * rhs.im
                ,lts.re * rhs.im + lts.im * rhs.re
                );
}
inline Complex  operator/( const Complex &lts, const Complex &rhs) {
  const Real d = rhs.re * rhs.re + rhs.im * rhs.im;
  return Complex((lts.re * rhs.re + lts.im * rhs.im)/d
                ,(lts.im * rhs.re - lts.re * rhs.im)/d
                );
}
inline Complex &operator+=(Complex &lts, const Complex &rhs) {
  lts.re += rhs.re; lts.im += rhs.im;
  return lts;
}
inline Complex &operator-=(Complex &lts, const Complex &rhs) {
  lts.re -= rhs.re; lts.im -= rhs.im;
  return lts;
}
inline Complex &operator*=(Complex &lts, const Complex &rhs) {
  return lts = lts * rhs;
}
inline Complex &operator/=(Complex &lts, const Complex &rhs) {
  return lts = lts / rhs;
}
inline Complex  operator-(const Complex &c) {
  return Complex(-c.re,-c.im);
}
inline bool    operator==(const Complex &lts, const Complex &rhs) {
  return (lts.re == rhs.re) && (lts.im == rhs.im);
}
inline bool    operator!=(const Complex &lts, const Complex &rhs) {
  return (lts.re != rhs.re) || (lts.im != rhs.im);
}
inline Real  arg2(const Complex &c) { // = arg(x)^2
  return c.re * c.re + c.im * c.im;
}
inline Real  arg( const Complex &c) {
  return sqrt(arg2(c));
}
inline Real  fabs(const Complex &c) { // = arg(c)
  return arg(c);
}
Complex sqrt(const Complex &c);
inline Complex sqr(const Complex &c) {
  return c*c;
}
inline Complex conjugate(const Complex &c) {
  return Complex(c.re,-c.im);
}
Complex exp( const Complex &c);
inline Complex log(const Complex &c) {
  const Polar p(c);
  return Complex(log(p.r), p.theta);
}
Complex pow( const Complex &c, const Complex &p);
Complex root(const Complex &c, const Complex &r);
Complex sin( const Complex &c);
Complex cos( const Complex &c);
Complex tan( const Complex &c);
Complex asin(const Complex &c);
Complex acos(const Complex &c);
Complex atan(const Complex &c);

void setToRandom(Complex &c, RandomGenerator &rnd = *RandomGenerator::s_stdGenerator);

String toString(const Complex &c, StreamSize precision=6, StreamSize width=0, FormatFlags flags=0);

Complex strtoc(const char    *s, char    **end);
Complex wcstoc(const wchar_t *s, wchar_t **end);

#if defined(_UNICODE)
#define _tcstoc wcstoc
#else
#define _tcstoc strtoc
#endif // _UNICODE

std::istream &operator>>(std::istream &in ,       Complex &c);
std::ostream &operator<<(std::ostream &out, const Complex &c);

std::wistream &operator>>(std::wistream &in ,       Complex &c);
std::wostream &operator<<(std::wostream &out, const Complex &c);

typedef FunctionTemplate<Complex,Complex>   ComplexFunction;
typedef VectorTemplate<Complex>             ComplexVector;

// assume r >= 1
ComplexVector roots(const Complex &c, UINT r);
