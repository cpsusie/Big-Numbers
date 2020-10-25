#pragma once

#include <MathUtil.h>
#include <NumberInterval.h>
#include "CubeTemplate.h"
#include "Point2D.h"

template<typename T> class Rectangle2DTemplate : public CubeTemplate<T, 2> {
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
  template<typename T1, typename T2> Rectangle2DTemplate(const PointTemplate<T1, 2> &topLeft, const PointTemplate<T2, 2> &bottomRight)
    : CubeTemplate<T, 2>(topLeft, bottomRight-topLeft)
  {
  }
  template<typename P, typename S> Rectangle2DTemplate(const PointTemplate<P, 2> &p0, const SizeTemplate<S, 2> &size)
    : CubeTemplate<T, 2>(p0, size)
  {
  }

#if defined(__ATLTYPES_H__)
  inline Rectangle2DTemplate(const CRect &r)
    : CubeTemplate<T, 2>((Point2D)r.TopLeft(), (Size2D)r.Size())
  {
  }
  inline explicit operator CRect() const {
    return CRect((CPoint)LT(), (CPoint)RB());
  }
#endif // __ATLTYPES_H__

  inline const T &getX()      const { return p0()[0];                 }
  inline const T &getY()      const { return p0()[1];                 }
  inline const T &getWidth()  const { return size()[0];               }
  inline const T &getHeight() const { return size()[1];               }
  inline const T &getLeft()   const { return p0()[0];                 }
  inline       T getRight()   const { return getLeft() + getWidth();  }
  inline const T &getTop()    const { return p0()[1];                 }
  inline       T  getBottom() const { return getTop()  + getHeight(); }

  inline       Point2DTemplate<T>  LT() const { return p0();                                        }
  inline       Point2DTemplate<T>  RT() const { return Point2DTemplate<T>(getRight(), getTop()   ); }
  inline       Point2DTemplate<T>  LB() const { return Point2DTemplate<T>(getLeft() , getBottom()); }
  inline       Point2DTemplate<T>  RB() const { return LT() + size();                               }

  inline T getMinX() const { return __super::getMin(0); }
  inline T getMaxX() const { return __super::getMax(0); }
  inline T getMinY() const { return __super::getMin(1); }
  inline T getMaxY() const { return __super::getMax(1); }

  inline NumberInterval<T> getXInterval() const { return NumberInterval<T>(getLeft(), getRight() ); }
  inline NumberInterval<T> getYInterval() const { return NumberInterval<T>(getTop() , getBottom()); }

  // Returns a Rectangle2DTemplate with non-negative width and non-positive height
  inline static Rectangle2DTemplate makeBottomUpRectangle(const Rectangle2DTemplate &rect) {
    return Rectangle2DTemplate(rect.getMinX(),rect.getMaxY(), rect.getMaxX() - rect.getMinX(), rect.getMinY() - rect.getMaxY());
  }

  // Returns a Rectangle2DTemplate with non-negative width and height
  inline static Rectangle2DTemplate makePositiveRectangle(const Rectangle2DTemplate &rect) {
    return Rectangle2DTemplate(rect.getMinX(),rect.getMinY(), rect.getMaxX() - rect.getMinX(), rect.getMaxY() - rect.getMinY());
  }

  inline T getArea() const {
    return getVolume();
  }
};

typedef Rectangle2DTemplate<float   > FloatRectangle2D;
typedef Rectangle2DTemplate<double  > Rectangle2D;
typedef Rectangle2DTemplate<Real    > RealRectangle2D;

template<typename T> class Point2DArrayTemplate : public PointArrayTemplate<T, 2> {
public:
  Point2DArrayTemplate() {
  }
  explicit Point2DArrayTemplate(size_t capacity)
    : PointArrayTemplate<T, 2>(capacity)
  {
  }
  template<template<typename,UINT> class VType, typename S> Point2DArrayTemplate(const CollectionBase<VType<S, 2> > &src)
    : PointArrayTemplate<T, 2>(src)
  {
  }

  Rectangle2DTemplate<T> getBoundingBox() const {
    return getBoundingCube();
  }
};

typedef Point2DArrayTemplate<float    > FloatPoint2DArray;
typedef Point2DArrayTemplate<double   > Point2DArray;
typedef Point2DArrayTemplate<Real     > RealPoint2DArray;

template<typename T> class Point2DRefArrayTemplate : public PointRefArrayTemplate<T, 2> {
public:
  Point2DRefArrayTemplate() {
  }
  Point2DRefArrayTemplate(Point2DArrayTemplate<T> &src)
    : PointRefArrayTemplate<T, 2>(src)
  {
  }
};

typedef Point2DRefArrayTemplate<float > FloatPoint2DRefArray;
typedef Point2DRefArrayTemplate<double> Point2DRefArray;
typedef Point2DRefArrayTemplate<Real  > RealPoint2DRefArray;
