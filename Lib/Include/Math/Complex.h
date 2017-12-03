#pragma once

#include <StrStream.h>
#include <MatrixTemplate.h>
#include "MathLib.h"
#include "Point2D.h"

class Complex {
public:
  Real re, im;
#ifdef LONGDOUBLE
  Complex(double _re, double _im = 0) : re(_re), im(_im) {
  }
#endif

  Complex() : re(0), im(0) {
  }
  Complex(int _re) : re(_re), im(0) {
  }
  Complex(UINT _re) : re(_re), im(0) {
  }
  Complex(const Real &_re, const Real &_im = 0) : re(_re), im(_im) {
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

Complex strtoc(const char    *s, char    **end);
Complex wcstoc(const wchar_t *s, wchar_t **end);

#ifdef _UNICODE
#define _tcstoc wcstoc
#else
#define _tcstoc strtoc
#endif // _UNICODE

istream &operator>>(istream &in , Complex &c);
ostream &operator<<(ostream &out, const Complex &c);

std::wistream &operator>>(std::wistream &in , Complex &c);
std::wostream &operator<<(std::wostream &out, const Complex &c);

StrStream &operator<<(StrStream &stream, const Complex &c);

typedef FunctionTemplate<Complex,Complex>   ComplexFunction;
typedef VectorTemplate<Complex>   ComplexVector;

ComplexVector roots(const Complex &c, int r);
