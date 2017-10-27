#pragma once

#include "MyUtil.h"         // MFC core and standard components
#include <Math.h>
#include <Math/MathLib.h>
#include <Math/Double80.h>

template<class T> class Size2DTemplate {
public:
  T cx, cy;
  inline Size2DTemplate() {
    cx = cy = 0;
  }
  inline Size2DTemplate(const T &_cx, const T&_cy) : cx(_cx), cy(_cy) {
  }
  inline Size2DTemplate operator-(const Size2DTemplate &s) const {
    return Size2DTemplate(cx-s.cx, cy-s.cy);
  }
  inline Size2DTemplate operator+(const Size2DTemplate &s) const {
    return Size2DTemplateSize2D(cx+s.cx, cy+s.cy);
  }
  friend inline Size2DTemplate operator*(const T &factor, const Size2DTemplate &s) {
    return Size2DTemplate(factor*s.cx, factor*s.cy);
  }
  inline Size2DTemplate operator*(const T &factor) const {
    return Size2DTemplate(cx*factor, cy*factor);
  }
  inline Size2DTemplate operator/(const T &factor) const {
    return Size2DTemplate(cx/factor, cy/factor);
  }
  inline Size2DTemplate &operator*=(const T &factor) {
    cx *= factor;
    cy *= factor;
    return *this;
  }
  inline Size2DTemplate &operator/=(const T &factor) {
    cx /= factor;
    cy /= factor;
    return *this;
  }

  inline T length() const {
    return sqrt(cx*cx + cy*cy);
  }
  inline T area() const {
    return cx*cy;
  }
  inline Size2DTemplate &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }

  inline bool operator==(const Size2DTemplate &s) const {
    return (cx==s.cx) && (cy==s.cy);
  }
  inline bool operator!=(const Size2DTemplate &s) const {
    return !(*this == s);
  }
  inline String toString(int precision = 3) const {
    return format(_T("(%s,%s)"), ::toString(cx, precision).cstr(), ::toString(cy, precision).cstr());
  }
};

typedef Size2DTemplate<float >   FloatSize2D;
typedef Size2DTemplate<double>   Size2D;
typedef Size2DTemplate<Double80> D80Size2D;
typedef Size2DTemplate<Real>     RealSize2D;

template<class T> class Point2DTemplate {
public:
  T x, y;
  inline Point2DTemplate() {
    x = y = 0;
  }

  inline Point2DTemplate(const T &_x, const T &_y) : x(_x), y(_y) {
  }

  explicit Point2DTemplate(const String &s) {
    DEFINEMETHODNAME;
    const _TUCHAR *cp = (_TUCHAR*)s.cstr();
    while(_istspace(*cp)) cp++;
    bool gotParentes = false;
    if(*cp == '(') {
      cp++;
      gotParentes = true;
    }
    const _TUCHAR *cpx = cp;
    cp = parseReal(cp);
    if(cp == NULL) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    while(_istspace(*cp)) cp++;
    switch(*cp) {
    case _T(','):
    case _T(';'):
      cp++;
      break;
    }
    const _TUCHAR *cpy = cp;
    cp = parseReal(cp);
    if(cp == NULL) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    if(gotParentes) {
      while(_istspace(*cp)) cp++;
      if(*cp != _T(')')) {
        throwInvalidArgumentException(method, _T("s=%s. missing ')'"), s.cstr());
      }
    }
    x = _ttof(cpx);
    y = _ttof(cpy);
  }

  inline Point2DTemplate(const Size2DTemplate<T> &s) : x(s.cx), y(s.cy) {
  }
//  Point2D(const POINTFX &p);

  inline Point2DTemplate operator-() const {
    return Point2DTemplate(-x, -y);
  }
  inline Point2DTemplate operator+(const Point2DTemplate &p) const {
    return Point2DTemplate(x+p.x, y+p.y);
  }
  inline Point2DTemplate operator-(const Point2DTemplate &p) const {
    return Point2DTemplate(x-p.x, y-p.y);
  }
  inline Point2DTemplate operator+(const Size2D &s) const {
    return Point2DTemplate(x+s.cx, y+s.cy);
  }
  inline Point2DTemplate operator-(const Size2D &s) const {
    return Point2DTemplate(x-s.cx, y-s.cy);
  }

  inline Point2DTemplate operator%(const Point2DTemplate &p) const { // x+=p1.x, y-=p1.y
    return Point2DTemplate(x+p.x, y-p.y);
  }
  inline Point2DTemplate operator*(const T &factor) const {
    return Point2DTemplate(x*factor, y*factor);
  }
  inline Point2DTemplate operator/(const T &factor) const {
    return Point2DTemplate(x/factor, y/factor);
  }

  inline T operator*(const Point2DTemplate &p) const {
    return x*p.x + y*p.y;
  }
  inline Point2DTemplate &operator+=(const Point2DTemplate &p) {
    x += p.x;
    y += p.y;
    return *this;
  }
  inline Point2DTemplate &operator-=(const Point2DTemplate &p) {
    x -= p.x;
    y -= p.y;
    return *this;
  }
  inline Point2DTemplate &operator+=(const Size2D &s) {
    x += s.cx;
    y += s.cy;
    return *this;
  }
  inline Point2DTemplate &operator-=(const Size2D &s) {
    x -= s.cx;
    y -= s.cy;
    return *this;
  }
  inline Point2DTemplate &operator*=(const T &factor) {
    x *= factor;
    y *= factor;
    return *this;
  }
  inline Point2DTemplate &operator/=(const T &factor) {
    x /= factor;
    y /= factor;
    return *this;
  }
  Point2DTemplate rotate(const T &rad) const {
    const T cs = cos(rad);
    const T sn = sin(rad);
    return Point2DTemplate(cs * x + -sn * y, sn * x + cs * y);
  }

  inline T length() const {
    return sqrt(x*x + y*y);
  }

  inline Point2DTemplate &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }

  inline bool operator==(const Point2DTemplate &p) const {
    return (x==p.x) && (y==p.y);
  }
  inline bool operator!=(const Point2DTemplate &p) const {
    return (x != p.x) || (y != p.y);
  }
  inline bool operator<(const Point2DTemplate &p) const {
    return (x < p.x) && (y < p.y);
  }
  inline bool operator<=(const Point2DTemplate &p) const {
    return (x <= p.x) && (y <= p.y);
  }
  inline String toString(int precision = 3) const {
    return format(_T("(%s,%s)"), ::toString(x, precision).cstr(), ::toString(y, precision).cstr());
  }
  String toXML() const {
    return format(_T("<point>\n<x>%s</x>\n<y>%s</y>\n</point>\n")
                 ,::toString(x).cstr(),::toString(y).cstr());
  }
};

typedef Point2DTemplate<float >   FloatPoint2D;
typedef Point2DTemplate<double>   Point2D;
typedef Point2DTemplate<Double80> D80Point2D;
typedef Point2DTemplate<Real>     RealPoint2D;

template <class T> class Line2DTemplate {
public:
  Point2DTemplate<T> m_p1, m_p2;
  Line2DTemplate(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2)
    : m_p1(p1)
    , m_p2(p2)
  {
  }
};

typedef Line2DTemplate<float >   FloatLine2D;
typedef Line2DTemplate<double>   Line2D;
typedef Line2DTemplate<Double80> D80Line2D;
typedef Line2DTemplate<Real>     RealLine2D;

template<class T> T distance(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2) {
  return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y));
}

template<class T> Point2DTemplate<T> pointOfIntersection(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2, bool &intersect) {
  const T A11 = line1.m_p1.y       - line1.m_p2.y;
  const T A12 = line1.m_p2.x       - line1.m_p1.x;
  const T A21 = line2.m_p1.y       - line2.m_p2.y;
  const T A22 = line2.m_p2.x       - line2.m_p1.x;

  const T d  = A11 * A22 - A12 * A21;
  if(d == 0) {
    intersect = false;
    return Point2DTemplate<T>(0,0);
  }
  intersect = true;

  const T B1  = line1.m_p1.x * A11 + line1.m_p1.y * A12;
  const T B2  = line2.m_p1.x * A21 + line2.m_p1.y * A22;

//  A11*x + A12*y = B1
//  A21*x + A22*y = B2
// d = | A11 A12 |  dx = | B1 A12 |  dy = | A11 B1 |
//     | A21 A22 |       | B2 A22 |       | A21 B2 |
// Cramers rule: (x,y) = (dx/d, dy/d)

  const T dx = B1  * A22 - B2  * A12;
  const T dy = A11 * B2  - A21 * B1;
  return Point2DTemplate<T>(dx/d, dy/d);
}

template<class T> T distanceFromLine(const Point2DTemplate<T> &lp0, const Point2DTemplate<T> &lp1, const Point2DTemplate<T> &p) {
  if(lp1 == lp0) {
    return distance(p,lp1);
  }
  Point2DTemplate<T> u = lp1 - lp0;
  const Point2DTemplate<T> d = p-lp0;
  u /= u.length();
  return (d - (d * u) * u).length();
}

template<class T> T distanceFromLineSegment(const Point2DTemplate<T> &lp0, const Point2DTemplate<T> &lp1, const Point2DTemplate<T> &p) {
  const Point2DTemplate<T> u = lp1 - lp0;
  if((p-lp0) * u < 0) {
    return distance(p, lp0);
  } else if((p-lp1) * u > 0) {
    return distance(p, lp1);
  } else {
    return distanceFromLine(lp0, lp1, p);
  }
}

template<class T> T distanceFromLine(const Line2DTemplate<T> &line, const Point2DTemplate<T> &p) {
  return distanceFromLine(line.m_p1, line.m_p2, p);
}

template<class T> T distanceFromLineSegment(const Line2DTemplate<T> &line, const Point2DTemplate<T> &p) {
  return distanceFromLineSegment(line.m_p1, line.m_p2, p);
}

template<class T> Point2DTemplate<T> unit(const Point2DTemplate<T> &p) {
  return Point2DTemplate<T>(p).normalize();
}

template<class T> Point2DTemplate<T> operator*(const T &factor, const Point2DTemplate<T> &p) {
  return Point2DTemplate<T>(p.x*factor, p.y*factor);
}

template<class T> T det(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2) {
  return p1.x*p2.y - p1.y*p2.x;
}

template<class T> bool isParallel(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2) {
  return det(line1.m_p2 - line1.m_p1, line2.m_p2 - line2.m_p1) == 0;
}

// angle in radians between p1 and p2
template<class T> T angle(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2) {
  const T l1 = p1.length();
  const T l2 = p2.length();
  if((l1 == 0) || (l2 == 0)) {
    return 0;
  } else {
    const T f = (p1 * p2) / (l1 * l2);
    if(f <= -1) {
      return M_PI;
    } else if(f >= 1) {
      return 0;
    } else {
      return acos(f) * sign(det(p1, p2));
    }
  }
}

class Point2DOperator {
public:
  virtual void apply(const Point2D &p) = 0;
};

typedef FunctionTemplate<Point2D, Point2D> Point2DFunction;
typedef FunctionTemplate<Point2D, Real>    Function2D;


//inline float fixedToFloat(const FIXED &x) {
//  return (float)x.value + (float)x.fract / 0x10000u;
//}

//FIXED floatToFixed(float x);
//MAT2 rotation(float degree);
//MAT2 getIdentity(float size = 1.0f);

