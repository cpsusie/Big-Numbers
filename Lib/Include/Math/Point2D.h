#pragma once

#include "MyUtil.h"         // MFC core and standard components
#include <Math.h>
#include <MathUtil.h>
#include "MathLib.h"
#include "Double80.h"
#include "Fixed.h"
template<typename T> class Size2DTemplate {
public:
  T cx, cy;
  inline Size2DTemplate() {
    cx = cy = 0;
  }
  template<typename S> Size2DTemplate(const Size2DTemplate<S> &s)
    : cx((T)s.cx), cy((T)s.cy)
  {
  }
  template<typename X, typename Y> Size2DTemplate(const X &_cx, const Y &_cy)
    : cx((T)_cx), cy((T)_cy)
  {
  }
  template<typename S> Size2DTemplate<T> &operator=(const Size2DTemplate<S> &s) {
    cx = (T)s.cx;
    cy = (T)s.cy;
    return *this;
  }
  template<typename S> Size2DTemplate<T> operator-(const Size2DTemplate<S> &s) const {
    return Size2DTemplate<T>(cx-s.cx, cy-s.cy);
  }
  template<typename S> Size2DTemplate<T> operator+(const Size2DTemplate<S> &s) const {
    return Size2DTemplate<T>(cx+s.cx, cy+s.cy);
  }
  template<typename S> Size2DTemplate<T> operator*(const S &factor) const {
    return Size2DTemplate<T>(cx*factor, cy*factor);
  }
  template<typename S> Size2DTemplate<T> operator/(const S &factor) const {
    return Size2DTemplate<T>(cx/factor, cy/factor);
  }
  template<typename S> Size2DTemplate<T> &operator*=(const S &factor) {
    const T tmp = (T)factor;
    cx *= tmp;
    cy *= tmp;
    return *this;
  }
  template<typename S> Size2DTemplate<T> &operator/=(const S &factor) {
    const T tmp = (T)factor;
    cx /= tmp;
    cy /= tmp;
    return *this;
  }

  inline T length() const {
    return (T)sqrt(cx*cx + cy*cy);
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

template<typename T, typename S> Size2DTemplate<S> operator*(const T &factor, const Size2DTemplate<S> &s) {
  return s * f;
}

typedef Size2DTemplate<float >   FloatSize2D;
typedef Size2DTemplate<double>   Size2D;
typedef Size2DTemplate<Double80> D80Size2D;
typedef Size2DTemplate<Real>     RealSize2D;

template<typename T> class Point2DTemplate {
public:
  T x, y;
  inline Point2DTemplate() {
    x = y = 0;
  }

  template<typename S> Point2DTemplate(const Point2DTemplate<S> &p)
    : x((T)p.x), y((T)p.y)
  {
  }
  template<typename X, typename Y> Point2DTemplate(const X &_x, const Y &_y)
    : x((T)_x), y((T)_y)
  {
  }
  template<typename S> inline Point2DTemplate(const Size2DTemplate<S> &s)
    : x((T)s.cx), y((T)s.cy)
  {
  }
  template<typename S> Point2DTemplate<T> &operator=(const Point2DTemplate<S> &p) {
    x = (T)p.x;
    y = (T)p.y;
    return *this;
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
    _TUCHAR *next = NULL;
    const T _x = _tcstor(cp, &next);
    if(next == NULL) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    cp = next;
    while(_istspace(*cp)) cp++;
    switch(*cp) {
    case _T(','):
    case _T(';'):
      cp++;
      break;
    }
    next = NULL;
    const T _y = _tcstor(cp, &next);
    if(next == NULL) {
      throwInvalidArgumentException(method, _T("s=%s"), s.cstr());
    }
    if(gotParentes) {
      while(_istspace(*cp)) cp++;
      if(*cp != _T(')')) {
        throwInvalidArgumentException(method, _T("s=%s. missing ')'"), s.cstr());
      }
    }
    x = _x;
    y = _y;
  }

  inline Point2DTemplate(const POINTFX &p) {
    x = fixedToFloat(p.x);
    y = fixedToFloat(p.y);
  }

  inline Point2DTemplate<T> operator-() const {
    return Point2DTemplate<T>(-x, -y);
  }
  template<typename S> Point2DTemplate<T> operator+(const Point2DTemplate<S> &p) const {
    return Point2DTemplate<T>(x+p.x, y+p.y);
  }
  template<typename S> Point2DTemplate<T> operator-(const Point2DTemplate<S> &p) const {
    return Point2DTemplate<T>(x-p.x, y-p.y);
  }
  template<typename S> Point2DTemplate<T> operator+(const Size2DTemplate<S> &s) const {
    return Point2DTemplate<T>(x+s.cx, y+s.cy);
  }
  template<typename S> Point2DTemplate operator-(const Size2DTemplate<S> &s) const {
    return Point2DTemplate<T>(x-s.cx, y-s.cy);
  }

  template<typename S> Point2DTemplate<T> operator%(const Point2DTemplate<S> &p) const { // x+=p1.x, y-=p1.y
    return Point2DTemplate<T>(x+p.x, y-p.y);
  }
  template<typename S> Point2DTemplate operator*(const S &factor) const {
    return Point2DTemplate(x*factor, y*factor);
  }
  template<typename S> Point2DTemplate operator/(const S &factor) const {
    return Point2DTemplate(x/factor, y/factor);
  }

  template<typename S> T operator*(const Point2DTemplate<S> &p) const {
    return (T)(x*p.x + y*p.y);
  }
  template<typename S> Point2DTemplate<T> &operator+=(const Point2DTemplate<S> &p) {
    x += (T)p.x;
    y += (T)p.y;
    return *this;
  }
  template<typename S> Point2DTemplate<T> &operator-=(const Point2DTemplate<S> &p) {
    x -= (T)p.x;
    y -= (T)p.y;
    return *this;
  }
  template<typename S> Point2DTemplate<T> &operator+=(const Size2DTemplate<S> &s) {
    x += (T)s.cx;
    y += (T)s.cy;
    return *this;
  }
  template<typename S> Point2DTemplate<T> &operator-=(const Size2DTemplate<S> &s) {
    x -= (T)s.cx;
    y -= (T)s.cy;
    return *this;
  }
  template<typename S> Point2DTemplate<T> &operator*=(const S &factor) {
    const T tmp = (T)factor;
    x *= tmp;
    y *= tmp;
    return *this;
  }
  template<typename S> Point2DTemplate<T> &operator/=(const S &factor) {
    const T tmp = (T)factor;
    x /= tmp;
    y /= tmp;
    return *this;
  }
  template<typename S> Point2DTemplate<T> rotate(const S &rad) const {
    const T cs = (T)cos(rad);
    const T sn = (T)sin(rad);
    return Point2DTemplate<T>(cs * x + -sn * y, sn * x + cs * y);
  }

  inline T length() const {
    return (T)sqrt(x*x + y*y);
  }

  inline Point2DTemplate<T> &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }
  template<typename S> T distance(const Point2DTemplate<S> &p) const {
    return (*this - p).length();
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
};

template<typename T1, typename T2> Point2DTemplate<T1> Min(const Point2DTemplate<T1> &p1, const Point2DTemplate<T2> &p2) {
  return Point2DTemplate<T1>(min((T1)p1.x, (T1)p2.x), min((T1)p1.y, (T1)p2.y));
}

template<typename T1, typename T2> Point2DTemplate<T1> Max(const Point2DTemplate<T1> &p1, const Point2DTemplate<T2> &p2) {
  return Point2DTemplate<T1>(max((T1)p1.x, (T1)p2.x), max((T1)p1.y, (T1)p2.y));
}

template<typename T, typename P> Point2DTemplate<P> operator*(const T &factor, const Point2DTemplate<P> &p) {
  return p * f;
}

template<typename T1, typename T2> T1 distance(const Point2DTemplate<T1> &p1, const Point2DTemplate<T2> &p2) {
  return p1.distance(p2);
}

typedef Point2DTemplate<float >   FloatPoint2D;
typedef Point2DTemplate<double>   Point2D;
typedef Point2DTemplate<Double80> D80Point2D;
typedef Point2DTemplate<Real>     RealPoint2D;

template<typename T> class Line2DTemplate {
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

template<typename T> Point2DTemplate<T> pointOfIntersection(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2, bool &intersect) {
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

template<typename T> T distanceFromLine(const Point2DTemplate<T> &lp0, const Point2DTemplate<T> &lp1, const Point2DTemplate<T> &p) {
  if(lp1 == lp0) {
    return distance(p,lp1);
  }
  Point2DTemplate<T> u = lp1 - lp0;
  const Point2DTemplate<T> d = p-lp0;
  u /= u.length();
  return (d - (d * u) * u).length();
}

template<typename T> T distanceFromLineSegment(const Point2DTemplate<T> &lp0, const Point2DTemplate<T> &lp1, const Point2DTemplate<T> &p) {
  const Point2DTemplate<T> u = lp1 - lp0;
  if((p-lp0) * u < 0) {
    return distance(p, lp0);
  } else if((p-lp1) * u > 0) {
    return distance(p, lp1);
  } else {
    return distanceFromLine(lp0, lp1, p);
  }
}

template<typename T> T distanceFromLine(const Line2DTemplate<T> &line, const Point2DTemplate<T> &p) {
  return distanceFromLine(line.m_p1, line.m_p2, p);
}

template<typename T> T distanceFromLineSegment(const Line2DTemplate<T> &line, const Point2DTemplate<T> &p) {
  return distanceFromLineSegment(line.m_p1, line.m_p2, p);
}

template<typename T> Point2DTemplate<T> unit(const Point2DTemplate<T> &p) {
  return Point2DTemplate<T>(p).normalize();
}

template<typename T> Point2DTemplate<T> operator*(const T &factor, const Point2DTemplate<T> &p) {
  return Point2DTemplate<T>(p.x*factor, p.y*factor);
}

template<typename T> T det(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2) {
  return p1.x*p2.y - p1.y*p2.x;
}

template<typename T> bool isParallel(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2) {
  return det(line1.m_p2 - line1.m_p1, line2.m_p2 - line2.m_p1) == 0;
}

// angle in radians between p1 and p2
template<typename T> T angle(const Point2DTemplate<T> &p1, const Point2DTemplate<T> &p2) {
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

typedef FunctionTemplate<Point2D, Point2D> FunctionR2R2;
typedef FunctionTemplate<Point2D, Real>    FunctionR2R1;


RealPoint2D findExtremum(Function &f, const RealInterval &interval, bool maximum);
