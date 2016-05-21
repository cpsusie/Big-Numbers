#pragma once

#include <StrStream.h>
#include <Point2D.h>
#include "Real.h"

class Complex {
private:
  void init(const _TUCHAR *s);

public:
  Real re, im;
#ifdef LONGDOUBLE
  Complex(double r, double i = 0) { re = r; im = i; }
#endif

  Complex() { re = im = 0; }
  Complex(int         r) { re = r; im = 0; }
  Complex(const Real &r, const Real &i = 0) { re = r; im = i; }
  explicit inline Complex(const String &s) {
    init((_TUCHAR*)(s.cstr()));
  }
  explicit inline Complex(const TCHAR *s) {
    init((_TUCHAR*)s);
  }

  Complex &operator-=(const Complex &rhs);
  Complex &operator+=(const Complex &rhs);
  Complex &operator*=(const Complex &rhs);
  Complex &operator/=(const Complex &rhs);

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

  static const Complex i;
  static const Complex one;
  static const Complex zero;
};

class Polar {
private:
  void init(Real x, Real y);
public:
  Real r, theta;
  Polar();
  Polar(const Complex &c);
  Polar(const Point2D &p);
  operator Complex() const;
  operator Point2D() const;
  String toString(int dec = 1, bool rad = false) const;
};

Complex  operator+( const Complex &lts, const Complex &rhs);
Complex  operator-( const Complex &lts, const Complex &rhs);
Complex  operator*( const Complex &lts, const Complex &rhs);
Complex  operator/( const Complex &lts, const Complex &rhs);
Complex  operator-( const Complex &c);

bool    operator==(const Complex &lts, const Complex &rhs);
bool    operator!=(const Complex &lts, const Complex &rhs);

Real    arg( const Complex &c);
Real    arg2(const Complex &c); // = arg(x)^2
Real    fabs(const Complex &c); // = arg(c)
Complex sqrt(const Complex &c);
Complex sqr( const Complex &c);
Complex conjugate(const Complex &c);
Complex exp( const Complex &c);
Complex log( const Complex &c);
Complex pow( const Complex &c, const Complex &p);
Complex root(const Complex &c, const Complex &r);
Complex sin( const Complex &c);
Complex cos( const Complex &c);
Complex tan( const Complex &c);
Complex asin(const Complex &c);
Complex acos(const Complex &c);
Complex atan(const Complex &c);

void setToRandom(Complex &c);

String toString(const Complex &c, int precision=6, int width=0, int flags=0);

tistream &operator>>(tistream &in , Complex &c);
tostream &operator<<(tostream &out, const Complex &c);

StrStream &operator<<(StrStream &stream, const Complex &c);
