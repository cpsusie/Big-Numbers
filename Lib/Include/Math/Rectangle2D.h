#pragma once

#include <MathUtil.h>
#include <NumberInterval.h>
#include "Point2D.h"
#include "CubeN.h"

template<typename T> class Rectangle2DTemplate {
public:
  Point2DTemplate<T> m_p;
  Size2DTemplate<T>  m_size;
  inline Rectangle2DTemplate() : m_p(0,0), m_size(0,0) {
  }
  template<typename X, typename Y, typename W, typename H> Rectangle2DTemplate(const X &x, const Y &y, const W &w, const H &h)
    : m_p(x,y), m_size(w, h)
  {
  }
  template<typename R> Rectangle2DTemplate(const Rectangle2DTemplate<R> &src)
    : m_p(src.m_p), m_size(src.m_size)
  {
  }
  template<typename T1, typename T2> Rectangle2DTemplate(const Point2DTemplate<T1> &topLeft, const Point2DTemplate<T2> &bottomRight)
    : m_p(topLeft)
    , m_size(bottomRight-topLeft)
  {
  }
  template<typename P, typename S> Rectangle2DTemplate(const Point2DTemplate<P> &p, const Size2DTemplate<S> &size)
    : m_p(p), m_size(size)
  {
  }
  template<typename C> Rectangle2DTemplate(const CubeN<C> &src) {
    if(src.dim() != 2) throwInvalidArgumentException(__TFUNCTION__, "src has dimension %u. must be 2", src.dim());
    const NumberInterval<C> &xi = src.getInterval(0), &yi = src.getInterval(1);
    m_p    = CPoint2D(xi.getFrom()  , yi.getFrom()  );
    m_size = Size2D(  xi.getLength(), yi.getLength());
  }

#if defined(__ATLTYPES_H__)
  inline Rectangle2DTemplate(const CRect &r) : m_p(r.TopLeft()), m_size(r.Size()) {
  }
  inline operator CRect() const {
    return CRect((CPoint)m_p, (CSize)m_size);
  }
#endif
  inline const T &getX() const {
    return m_p.x;
  }
  inline const T &getY() const {
    return m_p.y;
  }
  inline const T &getWidth() const {
    return m_size.cx;
  }
  inline const T &getHeight() const {
    return m_size.cy;
  }
  inline const Point2DTemplate<T> &getTopLeft() const {
    return m_p;
  }
  inline Point2DTemplate<T> getTopRight() const {
    return Point2DTemplate<T>(m_p.x + m_size.cx, m_p.y);
  }
  inline Point2DTemplate<T> getBottomLeft()  const {
    return Point2DTemplate<T>(m_p.x, m_p.y + m_size.cy);
  }
  inline Point2DTemplate<T> getBottomRight() const {
    return Point2DTemplate<T>(m_p.x + m_size.cx, m_p.y + m_size.cy);
  }
  inline Point2DTemplate<T> getCenter() const {
    return Point2DTemplate<T>(m_p.x + m_size.cx/2, m_p.y + m_size.cy/2);
  }
  inline const Size2DTemplate<T> &getSize() const {
    return m_size;
  }
  inline T getArea() const {
    return m_size.area();
  }
  inline T getMinX() const {
    return min(m_p.x, m_p.x + m_size.cx);
  }
  inline T getMaxX() const {
    return max(m_p.x, m_p.x + m_size.cx);
  }
  inline T getMinY() const {
    return min(m_p.y, m_p.y + m_size.cy);
  }
  inline T getMaxY() const {
    return max(m_p.y, m_p.y + m_size.cy);
  }
  inline NumberInterval<T> getXInterval() const {
    return NumberInterval<T>(m_p.x, m_p.x+m_size.cx);
  }
  inline NumberInterval<T> getYInterval() const {
    return NumberInterval<T>(m_p.y, m_p.y+m_size.cy);
  }
  inline operator CubeN<T>() const {
    return CubeN<T>(2).setInterval(0, getXInterval()).setInterval(1, getYInterval());
  }
  template<typename P> bool contains(const Point2DTemplate<P> &p) const {
    return (getMinX() <= (T)p.x) && ((T)p.x <= getMaxX()) && (getMinY() <= (T)p.y) && ((T)p.y <= getMaxY());
  }
  template<typename TP> Rectangle2DTemplate &operator+=(const Point2DTemplate<TP> &dp) {
    m_p.x += (T)dp.x; m_p.y += (T)dp.y;
    return *this;
  }
  template<typename TP> Rectangle2DTemplate &operator-=(const Point2DTemplate<TP> &dp) {
    m_p.x -= (T)dp.x; m_p.y -= (T)dp.y;
    return *this;
  }
  inline bool operator==(const Rectangle2DTemplate &r) const {
    return (m_p==r.m_p) && (m_size==r.m_size);
  }
  inline bool operator!=(const Rectangle2DTemplate &r) const {
    return !(*this == r);
  }

  inline bool contains(const Rectangle2DTemplate &rect) const {
    return contains(rect.getTopLeft()) && contains(rect.getBottomRight());
  }
  inline Point2DTemplate<T> getProjection(const Point2DTemplate<T> &p) const {
    return Point2DTemplate<T>(minMax(p.x, m_p.x, m_p.x + m_size.cx), minMax(p.y, m_p.y, m_p.y + m_size.cy));
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

template<typename T1, typename T2> Rectangle2DTemplate<T1> getUnion(const Rectangle2DTemplate<T1> &r1, const Rectangle2DTemplate<T2> &r2) {
  const T1 minX = min(r1.getMinX(), r2.getMinX()), maxX = max(r1.getMaxX(), r2.getMaxX());
  const T1 minY = min(r1.getMinY(), r2.getMinY()), maxY = max(r1.getMaxY(), r2.getMaxY());
  return Rectangle2DTemplate<T1>(minX, minY, maxX - minX, maxY - minY);
}

template<typename T> class Point2DTemplateArray : public CompactArray<Point2DTemplate<T> > {
public:
  Point2DTemplateArray() {
  }
  explicit Point2DTemplateArray(size_t capacity) : CompactArray(capacity) {
  }
  template<typename S> Point2DTemplateArray(const Point2DTemplateArray<S> &src) : CompactArray(src.size) {
    const size_t n = src.size();
    for(const Point2DTemplate<S> p : src) {
      add(Point2DTemplate<T>(p));
    }
  }
  template<typename S> Point2DTemplateArray<T> &operator=(const Point2DTemplateArray<S> &src) {
    if((void*)&src == (void*)this) {
      return *this;
    }
    clear(src.size());
    for(const Point2DTemplate<S> p : src) {
      add(Point2DTemplate<T>(p));
    }
    return *this;
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

typedef Point2DTemplateArray<float   > FloatPoint2DArray;
typedef Point2DTemplateArray<double  > Point2DArray;
typedef Point2DTemplateArray<Double80> D80Point2DArray;
typedef Point2DTemplateArray<Real>     RealPoint2DArray;

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
