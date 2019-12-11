#pragma once

#include <MathUtil.h>
#include <NumberInterval.h>
#include "Point2D.h"

template<typename T> class Rectangle2DTemplate {
public:
  T m_x, m_y, m_w, m_h;
  inline Rectangle2DTemplate() : m_x(0), m_y(0), m_w(0), m_h(0) {
  }
  inline Rectangle2DTemplate(const T &x, const T &y, const T &w, const T &h)
    : m_x(x), m_y(y), m_w(w), m_h(h)
  {
  }
  inline Rectangle2DTemplate(const Point2DTemplate<T> &topLeft, const Point2DTemplate<T> &bottomRight)
    : m_x(topLeft.x), m_y(topLeft.y)
    , m_w(bottomRight.x-topLeft.x), m_h(bottomRight.y-topLeft.y)
  {
  }
  inline Rectangle2DTemplate(const Point2DTemplate<T> &p, const Size2DTemplate<T> &size)
    : m_x(p.x    ), m_y(p.y    )
    , m_w(size.cx), m_h(size.cy)
  {
  }
  inline const T &getX() const {
    return m_x;
  }
  inline const T &getY() const {
    return m_y;
  }
  inline const T &getWidth() const {
    return m_w;
  }
  inline const T &getHeight() const {
    return m_h;
  }
  inline Point2DTemplate<T> getTopLeft() const {
    return Point2DTemplate<T>(m_x,m_y);
  }
  inline Point2DTemplate<T> getTopRight() const {
    return Point2DTemplate<T>(m_x + m_w, m_y);
  }
  inline Point2DTemplate<T> getBottomLeft()  const {
    return Point2DTemplate<T>(m_x, m_y + m_h);
  }
  inline Point2DTemplate<T> getBottomRight() const {
    return Point2DTemplate<T>(m_x + m_w, m_y + m_h);
  }
  inline Point2DTemplate<T> getCenter() const {
    return Point2DTemplate<T>(m_x + m_w/2, m_y + m_h/2);
  }
  inline Size2DTemplate<T> getSize() const {
    return Size2DTemplate<T>(m_w,m_h);
  }
  inline T getArea() const {
    return m_w * m_h;
  }
  inline T getMinX() const {
    return min(m_x, m_x + m_w);
  }
  inline T getMaxX() const {
    return max(m_x, m_x + m_w);
  }
  inline T getMinY() const {
    return min(m_y, m_y + m_h);
  }
  inline T getMaxY() const {
    return max(m_y, m_y + m_h);
  }
  inline NumberInterval<T> getXInterval() const {
    return NumberInterval<T>(getMinX(), getMaxX());
  }
  inline NumberInterval<T> getYInterval() const {
    return NumberInterval<T>(getMinY(), getMaxY());
  }
  inline bool contains(const Point2DTemplate<T> &p) const {
    return (getMinX() <= p.x) && (p.x <= getMaxX()) && (getMinY() <= p.y) && (p.y <= getMaxY());
  }
  inline Rectangle2DTemplate &operator+=(const Point2DTemplate<T> &dp) {
    m_x += dp.x; m_y += dp.y;
    return *this;
  }
  inline Rectangle2DTemplate &operator-=(const Point2DTemplate<T> &dp) {
    m_x -= dp.x; m_y -= dp.y;
    return *this;
  }
  inline bool operator==(const Rectangle2DTemplate &r) const {
    return (m_x==r.m_x) && (m_y==r.m_y) && (m_w==r.m_w) && (m_h==r.m_h);
  }
  inline bool operator!=(const Rectangle2DTemplate &r) const {
    return !(*this == r);
  }

  inline bool contains(const Rectangle2DTemplate &rect) const {
    return contains(rect.getTopLeft()) && contains(rect.getBottomRight());
  }
  inline Point2DTemplate<T> getProjection(const Point2DTemplate<T> &p) const {
    return Point2DTemplate<T>(minMax(p.x, m_x, m_x + m_w), minMax(p.y, m_y, m_y + m_h));
  }

  // Returns a Rectangle2DTemplate with non-negative width and non-positive height
  inline static Rectangle2DTemplate makeBottomUpRectangle(const Rectangle2DTemplate &rect) {
    return Rectangle2DTemplate(rect.getMinX(),rect.getMaxY(), rect.getMaxX() - rect.getMinX(), rect.getMinY() - rect.getMaxY());
  }

  // Returns a Rectangle2DTemplate with non-negative width and height
  inline static Rectangle2DTemplate makePositiveRectangle(const Rectangle2DTemplate &rect) {
    return Rectangle2DTemplate(rect.getMinX(),rect.getMinY(), rect.getMaxX() - rect.getMinX(), rect.getMaxY() - rect.getMinY());
  }
};

typedef Rectangle2DTemplate<float   > FloatRectangle2D;
typedef Rectangle2DTemplate<double  > Rectangle2D;
typedef Rectangle2DTemplate<Double80> D80Rectangle2D;
typedef Rectangle2DTemplate<Real    > RealRectangle2D;

template<typename T> Rectangle2DTemplate<T> getUnion(const Rectangle2DTemplate<T> &r1, const Rectangle2DTemplate<T> &r2) {
  const T minX = min(r1.getMinX(), r2.getMinX());
  const T minY = min(r1.getMinY(), r2.getMinY());
  const T maxX = max(r1.getMaxX(), r2.getMaxX());
  const T maxY = max(r1.getMaxY(), r2.getMaxY());
  return Rectangle2DTemplate<T>(minX,minY, maxX-minX,maxY-minY);
}

template<typename T> class Point2DArrayTemplate : public CompactArray<Point2DTemplate<T> > {
public:
  Point2DArrayTemplate() {
  }
  explicit Point2DArrayTemplate(size_t capacity) : CompactArray<Point2DTemplate<T> >(capacity) {
  }
  Rectangle2DTemplate<T> getBoundingBox() const {
    if(size() == 0) {
      return Rectangle2D();
    }

    const Point2DTemplate<T> *p   = &first();
    const Point2DTemplate<T> *end = &last();
    T minX = p->x, maxX = minX, minY = p->y, maxY = minY;
    while(p++ < end) {
      if(p->x < minX) minX = p->x; else if(p->x > maxX) maxX = p->x;
      if(p->y < minY) minY = p->y; else if(p->y > maxY) maxY = p->y;
    }
    return Rectangle2DTemplate<T>(minX,minY, maxX-minX, maxY-minY);
  }
};

typedef Point2DArrayTemplate<float   > FloatPoint2DArray;
typedef Point2DArrayTemplate<double  > Point2DArray;
typedef Point2DArrayTemplate<Double80> D80Point2DArray;
typedef Point2DArrayTemplate<Real>     RealPoint2DArray;

template<typename T> class Point2DRefArrayTemplate : public CompactArray<Point2DTemplate<T>*> {
public:
  String toString(int precision = 3) const {
    String result = _T("(");
    if(size()) {
      const Point2DTemplate<T> * const *p = &first();
      result += ::toString(*(p++), precision);
      for(size_t i = size()-1; i--;) {
        result += _T(",");
        result += ::toString(*(p++), precision);
      }
    }
    result += _T(")");
    return result;
  }
};

typedef Point2DRefArrayTemplate<float   > FloatPoint2DRefArray;
typedef Point2DRefArrayTemplate<double>   Point2DRefArray;
typedef Point2DRefArrayTemplate<Double80> D80Point2DRefArray;
typedef Point2DRefArrayTemplate<Real>     RealPoint2DRefArray;
