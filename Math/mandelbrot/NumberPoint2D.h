#pragma once

#include <Math.h>

template <class T> class NumberPoint2D {
public:
  T x, y;
  inline NumberPoint2D() {
    x = y = 0;
  }

  inline NumberPoint2D(T _x, T _y) : x(_x), y(_y) {
  }
#ifdef LONGDOUBLE
  inline NumberPoint2D(double _x, double _y) : x(_x), y(_y) {
  }
#endif
  inline NumberPoint2D(const CPoint &p) : x(p.x), y(p.y) {
  }

  inline NumberPoint2D operator-() const {
    return NumberPoint2D(-x, -y);
  }

  inline NumberPoint2D operator-(const NumberPoint2D &p) const {
    return NumberPoint2D(x-p.x, y-p.y);
  }

  inline NumberPoint2D operator+(const NumberPoint2D &p) const {
    return NumberPoint2D(x+p.x, y+p.y);
  }

  inline NumberPoint2D operator%(const NumberPoint2D &p) const { // x+=p1.x, y-=p1.y
    return NumberPoint2D(x+p.x, y-p.y);
  }
  inline NumberPoint2D operator*(T factor) const {
    return NumberPoint2D(x*factor, y*factor);
  }
  inline friend NumberPoint2D operator*(T factor, const NumberPoint2D &p) {
    return NumberPoint2D(p.x*factor, p.y*factor);
  }
  inline NumberPoint2D operator/(T factor) const {
    return NumberPoint2D(x/factor, y/factor);
  }

  inline T operator*(const NumberPoint2D &p) const {
    return x*p.x + y*p.y;
  }

  inline NumberPoint2D &operator+=(const NumberPoint2D &p) {
    x += p.x;
    y += p.y;
    return *this;
  }

  inline NumberPoint2D &operator-=(const NumberPoint2D &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }

  inline NumberPoint2D &operator*=(T factor) {
    x *= factor;
    y *= factor;
    return *this;
  }

  inline NumberPoint2D &operator/=(T factor) {
    x /= factor;
    y /= factor;
    return *this;
  }

  NumberPoint2D rotate(T rad) const {
    const T cs = cos(rad);
    const T sn = sin(rad);
    return NumberPoint2D(cs*x - sn*y, sn*x + cs*y);
  }

  inline T length() const {
    return sqrt(x*x + y*y);
  }

  inline NumberPoint2D &normalize() {
    const T l = length();
    if(l != 0) {
      x/=l; y/=l;
    }
    return *this;
  }
  inline operator CPoint() const {
    return CPoint(getInt(round(x)), getInt(round(y)));
  }

  inline bool operator==(const NumberPoint2D &p) const {
    return (x==p.x) && (y==p.y);
  }
  inline bool operator!=(const NumberPoint2D &p) const {
    return (x != p.x) || (y != p.y);
  }
  inline bool operator<(const NumberPoint2D &p) const {
    return (x < p.x) && (y < p.y);
  }
  inline bool operator<=(const NumberPoint2D &p) const {
    return (x <= p.x) && (y <= p.y);
  }
  inline String toString(int dec = 3) const {
    return format(_T("(%s,%s)"), ::toString(x, dec).cstr(), ::toString(y, dec).cstr());
  }
};
