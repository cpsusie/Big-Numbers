#pragma once

#include <math.h>
#include "MathLib.h"
#include "Double80.h"

template<typename T> class Size3DTemplate {
public:
  T cx, cy, cz;
  inline Size3DTemplate() {
    cx = cy = cz = 0;
  }
  template<typename S> Size3DTemplate(const Size3DTemplate<S> &src)
    : cx((T)src.cx), cy((T)src.cy), cz((T)src.cz)
  {
  }
  template<typename X,typename Y,typename Z> Size3DTemplate(const X &_cx, const Y &_cy, const Z &_cz)
    : cx((T)_cx), cy((T)_cy), cz((T)_cz)
  {
  }
  template<typename S> Size3DTemplate<T> &operator=(const Size3DTemplate<S> &s) {
    cx = (T)s.cx;
    cy = (T)s.cy;
    cz = (T)s.cz;
    return *this;
  }

  template<typename S> Size3DTemplate<T> operator-(const Size3DTemplate<S> &s) const {
    return Size3DTemplate<T>(cx-s.cx, cy-s.cy, cz-s.cz);
  }
  template<typename S> Size3DTemplate<T> operator+(const Size3DTemplate<S> &s) const {
    return Size3DTemplate<T>(cx+s.cx, cy+s.cy, cz+s.cz);
  }
  template<typename S> Size3DTemplate<T> operator*(const S &factor) const {
    return Size3DTemplate<T>(cx*factor, cy*factor, cz*factor);
  }
  template<typename S> Size3DTemplate<T> operator/(const S &factor) const {
    return Size3DTemplate<T>(cx/factor, cy/factor, cz/factor);
  }
  template<typename S> Size3DTemplate<T> &operator*=(const S &factor) {
    const T tmp = (T)factor;
    cx *= tmp;
    cy *= tmp;
    cz *= tmp;
    return *this;
  }
  template<typename S> Size3DTemplate<T> &operator/=(const S &factor) {
    const T tmp = (T)factor;
    cx /= tmp;
    cy /= tmp;
    cz /= tmp;
    return *this;
  }

  inline T length() const {
    return (T)sqrt(cx*cx + cy*cy + cz*cz);
  }
  inline T volume() const {
    return cx*cy*cz;
  }
  inline Size3DTemplate<T> &normalize() {
    const T l = length();
    if(l != 0) {
      *this /= l;
    }
    return *this;
  }

  inline bool operator==(const Size3DTemplate &s) const {
    return (cx == s.cx) && (cy == s.cy) && (cz == s.cz);
  }
  inline bool operator!=(const Size3DTemplate &s) const {
    return !(*this == s);
  }
  inline String toString(int precision = 3) const {
    return format(_T("(%s,%s,%s)"), ::toString(cx, precision).cstr(), ::toString(cy, precision).cstr(), ::toString(cz, precision).cstr());
  }
};

template<typename T, typename S> Size3DTemplate<S> operator*(const T &factor, const Size3DTemplate<S> &s) {
  return s * f;
}

typedef Size3DTemplate<float   > FloatSize3D;
typedef Size3DTemplate<double  > Size3D;
typedef Size3DTemplate<Double80> D80Size3D;
typedef Size3DTemplate<Real    > RealSize3D;

// A point in 3-dimensional space
template<typename T> class Point3DTemplate {
public:
  T x, y, z;
  inline Point3DTemplate() {
    x = y = z = 0;
  }
  template<typename S> Point3DTemplate(const Point3DTemplate<S> &p)
    : x((T)p.x), y((T)p.y), z((T)p.z)
  {
  }
  template<typename X,typename Y, typename Z> Point3DTemplate(const X &_x, const Y &_y, const Z &_z)
    : x((T)_x), y((T)_y), z((T)_z)
  {
  }
  template<typename S> Point3DTemplate(const Size3DTemplate<S> &s)
    : x((T)s.cx), y((T)s.cy), z((T)s.cz)
  {
  }
  template<typename S> Point3DTemplate<T> &operator=(const Point3DTemplate<S> &p) {
    x = (T)p.x; y = (T)p.y; z = (T)p.z; return *this;
  }
#if defined(__D3DX9MATH_H__)
  inline Point3DTemplate(const D3DXVECTOR3 &v) : x((T)v.x), y((T)v.y), z((T)v.z) {
  }
  inline Point3DTemplate<T> &operator=(const D3DXVECTOR3 &v) {
    x = (T)v.x; y = (T)v.y; z = (T)v.z; return *this;
  }
  inline operator D3DXVECTOR3() const {
    return D3DXVECTOR3((float)x, (float)y, (float)z);
  }
#endif

  inline Point3DTemplate<T> operator-() const {
    return Point3DTemplate<T>(-x, -y, -z);
  }
  template<typename S> Point3DTemplate<T> operator+(const Point3DTemplate<S> &p) const {
    return Point3DTemplate<T>(x+p.x, y+p.y, z+p.z);
  }
  template<typename S> Size3DTemplate<T> operator-(const Point3DTemplate<S> &p) const {
    return Size3DTemplate<T>(x-p.x, y-p.y, z-p.z);
  }
  template<typename S> Point3DTemplate<T> operator+(const Size3DTemplate<S> &s) const {
    return Point3DTemplate<T>(x+s.cx, y+s.cy, z+s.cz);
  }
  template<typename S> Point3DTemplate<T> operator-(const Size3DTemplate<S> &s) const {
    return Point3DTemplate<T>(x-s.cx, y-s.cy, z-s.cz);
  }

  template<typename S> Point3DTemplate<T> operator*(const S &factor) const {
    return Point3DTemplate<T>(x*factor, y*factor, z*factor);
  }
  template<typename S> Point3DTemplate<T> operator/(const S &factor) const {
    return Point3DTemplate<T>(x/factor, y/factor, z/factor);
  }

  template<typename S> T operator*(const Point3DTemplate<S> &p) const {
    return (T)(x*p.x + y*p.y + z*p.z);
  }
  template<typename S> Point3DTemplate<T> &operator+=(const Point3DTemplate<S> &p) {
    x += (T)p.x;
    y += (T)p.y;
    z += (T)p.z;
    return *this;
  }
  template<typename S> Point3DTemplate<T> &operator-=(const Point3DTemplate<S> &p) {
    x -= (T)p.x;
    y -= (T)p.y;
    z -= (T)p.z;
    return *this;
  }
  template<typename S> Point3DTemplate<T> &operator+=(const Size3DTemplate<S> &s) {
    x += (T)s.cx;
    y += (T)s.cy;
    z += (T)s.cz;
    return *this;
  }
  template<typename S> Point3DTemplate<T> &operator-=(const Size3DTemplate<S> &s) {
    x -= (T)s.cx;
    y -= (T)s.cy;
    z -= (T)s.cz;
    return *this;
  }
  template<typename S> Point3DTemplate<T> &operator*=(const S &factor) {
    const T tmp = (T)factor;
    x *= tmp;
    y *= tmp;
    z *= tmp;
    return *this;
  }
  template<typename S> Point3DTemplate &operator/=(const S &factor) {
    const T tmp = (T)factor;
    x /= tmp;
    y /= tmp;
    z /= tmp;
    return *this;
  }

  inline T length() const {
    return (T)sqrt(x*x + y*y + z*z);
  }

  Point3DTemplate<T> &normalize() {
    const T l = length();
    if(l != 0) {
	    *this /= l;
    }
    return *this;
  }

  template<typename S> T distance(const Point3DTemplate<S> &p) const {
    return (*this - p).length();
  }
  inline bool operator==(const Point3DTemplate &p) const {
    return (x==p.x) && (y==p.y) && (z==p.z);
  }
  inline bool operator!=(const Point3DTemplate &p) const {
    return !(*this == p);
  }
  inline bool operator<(const Point3DTemplate &p) const {
    return (x < p.x) && (y < p.y) && (z < p.z);
  }
  inline bool operator<=(const Point3DTemplate &p) const {
    return (x <= p.x) && (y <= p.y) && (z <= p.z);
  }
  inline String toString(int precision = 3) const {
    return format(_T("(%s,%s,%s)"),::toString(x, precision).cstr(),::toString(y, precision).cstr(),::toString(z, precision).cstr());
  }
};

template<typename T1, typename T2> Point3DTemplate<T1> Min(const Point3DTemplate<T1> &p1, const Point3DTemplate<T2> &p2) {
  return Point3DTemplate<T1>(min((T1)p1.x, (T1)p2.x), min((T1)p1.y, (T1)p2.y), min((T1)p1.z, (T1)p2.z));
}

template<typename T1, typename T2> Point3DTemplate<T1> Max(const Point3DTemplate<T1> &p1, const Point3DTemplate<T2> &p2) {
  return Point3DTemplate<T1>(max((T1)p1.x, (T1)p2.x), max((T1)p1.y, (T1)p2.y), max((T1)p1.z, (T1)p2.z));
}

template<typename F, typename P> Point3DTemplate<P> operator*(const F &factor, const Point3DTemplate<P> &p) {
  return p * f;
}

template<typename T1, typename T2> T1 distance(const Point3DTemplate<T1> &p1, const Point3DTemplate<T2> &p2) {
  return p1.distance(p2);
}

// angle in radians between p1 and p2
template<typename T, typename S> T angle(const Point3DTemplate<T> &p1, const Point3DTemplate<S> &p2) {
  const T l1 = p1.length();
  const T l2 = (T)p2.length();
  if((l1 == (T)0) || (l2 == (T)0)) {
    return (T)0;
  } else {
    const T f = (p1 * p2) / (l1 * l2);
    if(f <= -1) {
      return (T)M_PI;
    } else if(f >= 1) {
      return (T)0;
    } else {
//      debugLog("l1:%le, l2:%le, p1:%s, p2:%s\n", l1, l2, p1.toString(10).cstr(), p2.toString(10).cstr());
      return (T)acos(f);
    }
  }
}

template<typename T1, typename T2> Point3DTemplate<T1> cross(const Point3DTemplate<T1> &a, const Point3DTemplate<T2> &b) {
  return Point3DTemplate<T1>(a.y*b.z - a.z*b.y
                            ,a.z*b.x - a.x*b.z
                            ,a.x*b.y - a.y*b.x
                            );
}

typedef Point3DTemplate<float   > FloatPoint3D;
typedef Point3DTemplate<double  > Point3D;
typedef Point3DTemplate<Double80> D80Point3D;
typedef Point3DTemplate<Real    > RealPoint3D;

typedef FunctionTemplate<Point3D, Point3D> FunctionR3R3;
typedef FunctionTemplate<Point3D, Real>    FunctionR3R1;
