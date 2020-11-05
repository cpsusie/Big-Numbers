#pragma once

#include <Math.h>
#include <MathUtil.h>
#include "MathLib.h"
#include "Fixed.h"
#include "PointTemplate.h"

template<typename T> class Size2DTemplate : public SizeTemplate<T, 2> {
public:
  inline Size2DTemplate() : SizeTemplate<T, 2>(0,0) {
  }
  template<typename S> Size2DTemplate(const FixedDimensionVector<S, 2> &v)
    : SizeTemplate<T, 2>(v[0],v[1])
  {
  }
  template<typename X, typename Y> Size2DTemplate(const X &cx, const Y &cy)
    : SizeTemplate<T, 2>(cx, cy)
  {
  }
  template<typename S> Size2DTemplate<T> &operator=(const FixedDimensionVector<S, 2> &v) {
    __super::operator=(v);
    return *this;
  }

#if defined(__ATLTYPES_H__)
  inline Size2DTemplate(const CSize &s)
    : SizeTemplate<T, 2>(s.cx,s.cy)
  {
  }
  inline Size2DTemplate<T> &operator=(const CSize &s) {
    cx() = (T)s.cx; cy() = (T)s.cy;
    return *this;
  }
  inline explicit operator CSize() const {
    return CSize((int)round(cx()), (int)round(cy()));
  }
#endif // __ATLTYPES_H__

  inline       T &cx()       { return (*this)[0]; }
  inline       T &cy()       { return (*this)[1]; }
  inline const T &cx() const { return (*this)[0]; }
  inline const T &cy() const { return (*this)[1]; }

  Size2DTemplate                      operator*(const T &k) const {
    return __super::operator*(k);
  }
  Size2DTemplate                      operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> T              operator*(const FixedDimensionVector<S, 2> &v) const {
    return __super::operator*(v);
  }
};

typedef Size2DTemplate<float >   FloatSize2D;
typedef Size2DTemplate<double>   Size2D;
typedef Size2DTemplate<Real>     RealSize2D;

template<typename T> class Point2DTemplate : public PointTemplate<T, 2> {
public:
  inline Point2DTemplate() : PointTemplate<T, 2>(0,0) {
  }
  template<typename S> Point2DTemplate(const FixedDimensionVector<S, 2> &v)
    : PointTemplate<T, 2>(v[0],v[1])
  {
  }
  template<typename X, typename Y> Point2DTemplate(const X &x, const Y &y)
    : PointTemplate<T, 2>(x, y)
  {
  }
  inline Point2DTemplate(const POINTFX &p)
    : PointTemplate<T, 2>(fixedToFloat(p.x), fixedToFloat(p.y))
  {
  }
  template<typename S> Point2DTemplate<T> &operator=(const FixedDimensionVector<S, 2> &v) {
    __super::operator=(v);
    return *this;
  }

#if defined(__ATLTYPES_H__)
  inline Point2DTemplate(const CPoint &p)
    : PointTemplate<T, 2>(p.x, p.y)
  {
  }
  inline Point2DTemplate<T> &operator=(const CPoint &p) {
    x() = (T)p.x; y() = (T)p.y;
    return *this;
  }
  inline explicit operator CPoint() const {
    return CPoint((int)round(x()), (int)round(y()));
  }
#endif // __ATLTYPES_H__

#if defined(__D3DX9MATH_H__)
  inline Point2DTemplate(const D3DXVECTOR2 &v)
    : PointTemplate<T, 2>(v.x, v.y)
  {
  }
  inline Point2DTemplate<T> &operator=(const D3DXVECTOR2 &v) {
    x() = x; y() = v.y;
    return *this;
  }
  inline operator D3DXVECTOR2() const {
    return D3DXVECTOR2((float)x(), (float)y());
  }
#endif // __D3DX9MATH_H__

  inline       T &x()       { return (*this)[0]; }
  inline       T &y()       { return (*this)[1]; }
  inline const T &x() const { return (*this)[0]; }
  inline const T &y() const { return (*this)[1]; }

  Point2DTemplate                        operator*(const T &k) const {
    return __super::operator*(k);
  }
  Point2DTemplate                        operator/(const T &k) const {
    return __super::operator/(k);
  }
  template<typename S> Point2DTemplate   operator+(const FixedDimensionVector<S, 2> &v) const {
    return __super::operator+(v);
  }
  template<typename S> Point2DTemplate   operator-(const FixedDimensionVector<S, 2> &v) const {
    return __super::operator-(v);
  }
  template<typename S> Size2DTemplate<T> operator-(const Point2DTemplate<S>            &p) const {
    return __super::operator-(p);
  }
  // Return dot product = *this * v
  template<typename S> T                 operator*(const FixedDimensionVector<S, 2> &v) const {
    return __super::operator*(v);
  }
  Point2DTemplate                        operator-() const {
    return __super::operator-();
  }

  // x+=p1.x, y-=p1.y
  template<typename S> Point2DTemplate<T> operator%(const Point2DTemplate<S>           &p) const {
    return Point2DTemplate<T>(x()+(T)p.x(), y()-(T)p.y());
  }
};

typedef Point2DTemplate<float >   FloatPoint2D;
typedef Point2DTemplate<double>   Point2D;
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
typedef Line2DTemplate<Real>     RealLine2D;

template<typename T> Point2DTemplate<T> pointOfIntersection(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2, bool &intersect) {
  const T A11 = line1.m_p1.y - line1.m_p2.y;
  const T A12 = line1.m_p2.x - line1.m_p1.x;
  const T A21 = line2.m_p1.y - line2.m_p2.y;
  const T A22 = line2.m_p2.x - line2.m_p1.x;

  const T d  = A11 * A22 - A12 * A21;
  if(d == 0) {
    intersect = false;
    return Point2DTemplate<T>(0,0);
  }
  intersect = true;

  const T B1 = line1.m_p1.x * A11 + line1.m_p1.y * A12;
  const T B2 = line2.m_p1.x * A21 + line2.m_p1.y * A22;

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
  const Size2DTemplate<T> u = (lp1 - lp0).normalize();
  const Size2DTemplate<T> d = (p   - lp0);
  return (d - (d * u) * u).length();
}

template<typename T> T distanceFromLineSegment(const Point2DTemplate<T> &lp0, const Point2DTemplate<T> &lp1, const Point2DTemplate<T> &p) {
  const Size2DTemplate<T> u = lp1 - lp0;
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

template<typename T> bool isParallel(const Line2DTemplate<T> &line1, const Line2DTemplate<T> &line2) {
  return det(line1.m_p2 - line1.m_p1, line2.m_p2 - line2.m_p1) == 0;
}

typedef ValueOperatorTemplate<     Point2D         > Point2DOperator;

typedef FunctionTemplate<Real        , Point2D     > FunctionR1R2;
typedef FunctionTemplate<Point2D     , Real        > FunctionR2R1;

typedef FunctionTemplate<FloatPoint2D, FloatPoint2D> FloatFunctionR2R2;
typedef FunctionTemplate<Point2D     , Point2D     > FunctionR2R2;
typedef FunctionTemplate<RealPoint2D , RealPoint2D > RealFunctionR2R2;

RealPoint2D findExtremum(Function &f, const RealInterval &interval, bool maximum);
