#include "pch.h"
#include <Math/Complex.h>

Polar::Polar() {
  r = theta = 0;
}

Polar::Polar(const Point2D &p) {
  init(p.x, p.y);
}

Polar::operator Point2D() const {
  return Point2D(getDouble(r*cos(theta)), getDouble(r*sin(theta)));
}

Polar::Polar(const Complex &c) {
  init(c.re, c.im);
}

void Polar::init(Real x, Real y) {
  r = sqrt(x*x + y*y);
  theta = atan2(y, x);
}

Polar::operator Complex() const {
  return Complex(r*cos(theta), r*sin(theta));
}

String Polar::toString(int dec, bool rad) const {
  return format(_T("(%s, %s)"), ::toString(r, dec).cstr(), ::toString(rad ? theta : RAD2GRAD(theta), dec).cstr());
}

