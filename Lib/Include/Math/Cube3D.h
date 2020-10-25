#pragma once

#include <NumberInterval.h>
#include "CubeTemplate.h"
#include "Point3D.h"

template<typename T> class Cube3DTemplate : public CubeTemplate<T,3> {
public:
  inline Cube3DTemplate() {
  }

  template<typename X, typename Y, typename Z, typename W, typename D, typename H> Cube3DTemplate(const X &x, const Y &y, const Z &z, const W &w, const D &d, const H &h)
    : CubeTemplate<T,2>(Point3DTemplate<T>(x,y,z), Size3DTemplate<T>(w,d,h))
  {
  }
  template<typename S> Cube3DTemplate(const CubeTemplate<S,3> &src)
    : CubeTemplate<T, 3>(src)
  {
  }
  template<typename T1, typename T2> Cube3DTemplate(const PointTemplate<T1,3> &lbn, const PointTemplate<T2,3> &rtf)
    : CubeTemplate<T,3>(lbn, rtf-lbn)
  {
  }
  template<typename P, typename S> Cube3DTemplate(const PointTemplate<P,3> &p0, const SizeTemplate<S,3> &size)
    : CubeTemplate<T,3>(p0, size)
  {
  }

  inline const T &getX()      const { return p0()[0];                   }
  inline const T &getY()      const { return p0()[1];                   }
  inline const T &getZ()      const { return p0()[2];                   }
  inline const T &getWidth()  const { return size()[0];                 }
  inline const T &getDepth()  const { return size()[1];                 }
  inline const T &getHeight() const { return size()[2];                 }
  inline const T &getLeft()   const { return getX();                    }
  inline       T getRight()   const { return getLeft()   + getWidth();  }
  inline const T &getNear()   const { return getY();                    }
  inline       T getFar()     const { return getNear()   + getDepth();  }
  inline const T &getBottom() const { return getZ();                    }
  inline       T getTop()     const { return getBottom() + getHeight(); }

  inline       Point3DTemplate<T>  LBN() const { return p0();                                                   }
  inline       Point3DTemplate<T>  RBN() const { return Point3DTemplate<T>(getRight(), getNear(), getBottom()); }
  inline       Point3DTemplate<T>  LBF() const { return Point3DTemplate<T>(getLeft() , getFar() , getBottom()); }
  inline       Point3DTemplate<T>  RBF() const { return Point3DTemplate<T>(getRight(), getFar() , getBottom()); }
  inline       Point3DTemplate<T>  LTN() const { return Point3DTemplate<T>(getLeft() , getNear(), getTop()   ); }
  inline       Point3DTemplate<T>  RTN() const { return Point3DTemplate<T>(getRight(), getNear(), getTop()   ); }
  inline       Point3DTemplate<T>  LTF() const { return Point3DTemplate<T>(getLeft() , getFar() , getTop()   ); }
  inline       Point3DTemplate<T>  RTF() const { return LBN() + size();                                         }

  inline T getMinX() const { return __super::getMin(0); }
  inline T getMaxX() const { return __super::getMax(0); }
  inline T getMinY() const { return __super::getMin(1); }
  inline T getMaxY() const { return __super::getMax(1); }
  inline T getMinZ() const { return __super::getMin(2); }
  inline T getMaxZ() const { return __super::getMax(2); }

  inline NumberInterval<T> getXInterval() const { return NumberInterval<T>(getLeft()  , getRight()); }
  inline NumberInterval<T> getYInterval() const { return NumberInterval<T>(getNear()  , getFar()  ); }
  inline NumberInterval<T> getZInterval() const { return NumberInterval<T>(getBottom(), getTop()  ); }
};

typedef Cube3DTemplate<float>  FloatCube3D;
typedef Cube3DTemplate<double> Cube3D;
typedef Cube3DTemplate<Real>   RealCube3D;

template<typename T> class Point3DArrayTemplate : public PointArrayTemplate<T, 3> {
public:
  Point3DArrayTemplate() {
  }
  explicit Point3DArrayTemplate(size_t capacity)
    : PointArrayTemplate<T, 3>(capacity)
  {
  }
  template<template<typename,UINT> class VType, typename S> Point3DArrayTemplate(const CollectionBase<VType<S, 3> > &src)
    : PointArrayTemplate<T, 3>(src)
  {
  }

  Cube3DTemplate<T> getBoundingBox() const {
    return getBoundingCube();
  }
};

typedef Point3DArrayTemplate<float   > FloatPoint3DArray;
typedef Point3DArrayTemplate<double  > Point3DArray;
typedef Point3DArrayTemplate<Real    > RealPoint3DArray;
