#pragma once

#include <Math.h>
#include <MathUtil.h>
#include "MathLib.h"
#include "PointTemplate.h"

template<typename T> class Size3DTemplate : public SizeTemplate<T, 3> {
public:
  inline Size3DTemplate() : SizeTemplate<T, 3>(0,0,0) {
  }
  template<typename S> Size3DTemplate(const FixedDimensionVector<S, 3> &v)
    : SizeTemplate<T, 3>(v[0],v[1],v[2])
  {
  }
  template<typename X, typename Y, typename Z> Size3DTemplate(const X &cx, const Y &cy, const Z &cz)
    : SizeTemplate<T, 3>(cx, cy, cz)
  {
  }
  template<typename S> Size3DTemplate &operator=(const FixedDimensionVector<S, 3> &v) {
    __super::operator=(v);
    return *this;
  }

  inline       T &cx()       { return (*this)[0]; }
  inline       T &cy()       { return (*this)[1]; }
  inline       T &cz()       { return (*this)[2]; }
  inline const T &cx() const { return (*this)[0]; }
  inline const T &cy() const { return (*this)[1]; }
  inline const T &cz() const { return (*this)[2]; }
};


typedef Size3DTemplate<float   > FloatSize3D;
typedef Size3DTemplate<double  > Size3D;
typedef Size3DTemplate<Real    > RealSize3D;

// A point in 3-dimensional space
template<typename T> class Point3DTemplate : public PointTemplate<T, 3> {
public:
  inline Point3DTemplate() : PointTemplate<T, 3>(0,0,0) {
  }
  template<typename S> Point3DTemplate(const FixedDimensionVector<S, 3> &v)
    : PointTemplate<T, 3>(v[0],v[1],v[2])
  {
  }
  template<typename X, typename Y, typename Z> Point3DTemplate(const X &x, const Y &y, const Z &z)
    : PointTemplate<T, 3>(x, y, z)
  {
  }
  template<typename S> Point3DTemplate &operator=(const FixedDimensionVector<S, 3> &v) {
    __super::operator=(v);
    return *this;
  }

#if defined(__D3DX9MATH_H__)
  inline Point3DTemplate(const D3DXVECTOR3 &v)
    : PointTemplate<T, 3>(v.x, v.y, v.z)
  {
  }
  inline Point3DTemplate &operator=(const D3DXVECTOR3 &v) {
    x() = v.x; y() = v.y; z() = v.z;
    return *this;
  }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3((float)x(), (float)y(), (float)z());
  }
#endif // __D3DX9MATH_H__

  inline       T &x()       { return (*this)[0];  }
  inline       T &y()       { return (*this)[1];  }
  inline       T &z()       { return (*this)[2];  }
  inline const T &x() const { return (*this)[0];  }
  inline const T &y() const { return (*this)[1];  }
  inline const T &z() const { return (*this)[2];  }

  template<typename S> Point3DTemplate operator+(const FixedDimensionVector<S, 3> &v) const {
    return __super::operator+(v);
  }
  template<typename S> Point3DTemplate operator-(const FixedDimensionVector<S, 3> &v) const {
    return __super::operator-(v);
  }
  template<typename S> Size3DTemplate<T>  operator-(const Point3DTemplate<S>           &p) const {
    return __super::operator-(p);
  }
  // Return dot product = *this * v
  template<typename S> T                  operator*(const FixedDimensionVector<S, 3> &v) const {
    return __super::operator*(v);
  }
};

typedef Point3DTemplate<float   >          FloatPoint3D;
typedef Point3DTemplate<double  >          Point3D;
typedef Point3DTemplate<Real    >          RealPoint3D;

typedef FunctionTemplate<Real   , Point3D> FunctionR1R3;
typedef FunctionTemplate<Point3D, Real   > FunctionR3R1;
typedef FunctionTemplate<Point3D, Point3D> FunctionR3R3;
