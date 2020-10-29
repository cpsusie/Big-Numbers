#pragma once

#include <MathUtil.h>
#include <NumberInterval.h>
#include "CubeTemplate.h"
#include "Point2D.h"

template<typename T> class Rectangle2DTemplate : public CubeTemplate<T, 2> {
#if defined(__ATLTYPES_H__)
private:
  static inline CPoint BottomLeft(const CRect &r) {
    return CPoint(r.left, r.bottom);
  }
  static inline CPoint TopRight(const CRect &r) {
    return CPoint(r.right, r.top);
  }
  static inline CSize Size(const CRect &r) {
    return TopRight(r) - BottomLeft(r);
  }
#endif // __ATLTYPES_H__

public:
  inline Rectangle2DTemplate() {
  }
  template<typename X, typename Y, typename W, typename H> Rectangle2DTemplate(const X &x, const Y &y, const W &w, const H &h)
    : CubeTemplate<T, 2>(Point2DTemplate<T>(x,y), Size2DTemplate<T>(w,h))
  {
  }
  template<typename S> Rectangle2DTemplate(const CubeTemplate<S, 2> &src)
    : CubeTemplate<T, 2>(src)
  {
  }
  template<typename T1, typename T2> Rectangle2DTemplate(const PointTemplate<T1, 2> &lb, const PointTemplate<T2, 2> &rt)
    : CubeTemplate<T, 2>(lb, rt-lb)
  {
  }
  template<typename P, typename S> Rectangle2DTemplate(const PointTemplate<P, 2> &p0, const SizeTemplate<S, 2> &size)
    : CubeTemplate<T, 2>(p0, size)
  {
  }

#if defined(__ATLTYPES_H__)
  inline Rectangle2DTemplate(const CRect &r)
    : CubeTemplate<T, 2>(Point2D(BottomLeft(r)), Size2D(Size(r)))
  {
  }
  inline explicit operator CRect() const {
    return CRect((CPoint)LB(), (CPoint)RT());
  }
#endif // __ATLTYPES_H__

  inline const T &getX()      const { return p0()[0];                   }
  inline const T &getY()      const { return p0()[1];                   }
  inline const T &getWidth()  const { return size()[0];                 }
  inline const T &getHeight() const { return size()[1];                 }
  inline const T &getLeft()   const { return getX();                    }
  inline       T  getRight()  const { return getLeft()   + getWidth();  }
  inline const T &getBottom() const { return getY();                    }
  inline const T  getTop()    const { return getBottom() + getHeight(); }

  inline       Point2DTemplate<T>  LB() const { return p0();                                        }
  inline       Point2DTemplate<T>  RB() const { return Point2DTemplate<T>(getRight(), getBottom()); }
  inline       Point2DTemplate<T>  LT() const { return Point2DTemplate<T>(getLeft() , getTop());    }
  inline       Point2DTemplate<T>  RT() const { return p0() + size();                               }

  inline T getMinX() const { return __super::getMin(0); }
  inline T getMaxX() const { return __super::getMax(0); }
  inline T getMinY() const { return __super::getMin(1); }
  inline T getMaxY() const { return __super::getMax(1); }

  inline NumberInterval<T> getXInterval() const { return getInterval(0); }
  inline NumberInterval<T> getYInterval() const { return getInterval(1); }

  // Returns a Rectangle2DTemplate with non-negative width and non-positive height
  inline static Rectangle2DTemplate makeBottomUpRectangle(const Rectangle2DTemplate &rect) {
    const T minX = rect.getMinX(), maxX = rect.getMaxX(), minY = rect.getMinY(), maxY = rect.getMaxY();
    return Rectangle2DTemplate(minX, maxY, maxX - minX, minY - maxY);
  }

  // Returns a Rectangle2DTemplate with non-negative width and height
  inline static Rectangle2DTemplate makePositiveRectangle(const Rectangle2DTemplate &rect) {
    const T minX = rect.getMinX(), maxX = rect.getMaxX(), minY = rect.getMinY(), maxY = rect.getMaxY();
    return Rectangle2DTemplate(minX, minY, maxX - minX, maxY - minY);
  }

  inline T getArea() const {
    return getVolume();
  }
};

typedef Rectangle2DTemplate<float            > FloatRectangle2D;
typedef Rectangle2DTemplate<double           > Rectangle2D;
typedef Rectangle2DTemplate<Real             > RealRectangle2D;

typedef PointArrayTemplate<FloatPoint2D    ,2> FloatPoint2DArray;
typedef PointArrayTemplate<Point2D         ,2> Point2DArray;
typedef PointArrayTemplate<RealPoint2D     ,2> RealPoint2DArray;

typedef PointRefArrayTemplate<FloatPoint2D, 2> FloatPoint2DRefArray;
typedef PointRefArrayTemplate<Point2D     , 2> Point2DRefArray;
typedef PointRefArrayTemplate<RealPoint2D , 2> RealPoint2DRefArray;
