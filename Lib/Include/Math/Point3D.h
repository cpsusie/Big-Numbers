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
  template<typename TS> inline Size3DTemplate(const Size3DTemplate<TS> &src)
    : cx((T)src.cx), cy((T)src.cy), cz((T)src.cz) {
  }
  inline Size3DTemplate(const T &_cx, const T&_cy, const T&_cz) : cx(_cx), cy(_cy), cz(_cz) {
  }
  template<typename TS> inline Size3DTemplate operator-(const Size3DTemplate<TS> &s) const {
    return Size3DTemplate(cx - (T)s.cx, cy - (T)s.cy, cz - (T)s.cz);
  }
  template<typename TS> inline Size3DTemplate operator+(const Size3DTemplate<TS> &s) const {
    return Size3DTemplate(cx + (T)s.cx, cy + (T)s.cy, cz + (T)s.cz);
  }
  template<typename TS> inline Size3DTemplate operator*(const TS &d) const {
    const T factor = (T)d;
    return Size3DTemplate(cx * factor, cy * factor, cz * factor);
  }
  template<typename TS> inline Size3DTemplate operator/(const TS &d) const {
    const T factor = (T)d;
    return Size3DTemplate(cx / factor, cy / factor, cz / factor);
  }
  template<typename TS> inline Size3DTemplate &operator*=(const TS &d) {
    const T factor = (T)d;
    cx *= factor; cy *= factor; cz *= factor;
    return *this;
  }
  template<typename TS> inline Size3DTemplate &operator/=(const TS &d) {
    const T factor = (T)d;
    cx /= factor; cy /= factor; cz /= factor;
    return *this;
  }

  inline T length() const {
    return sqrt(cx * cx + cy * cy + cz * cz);
  }
  inline T volume() const {
    return cx * cy * cz;
  }
  inline Size3DTemplate &normalize() {
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

typedef Size3DTemplate<float   > FloatSize3D;
typedef Size3DTemplate<double  > Size3D;
typedef Size3DTemplate<Double80> D80Size3D;
typedef Size3DTemplate<Real    > RealSize3D;

// A point in 3-dimensional space
template<typename T> class Point3DTemplate {
public:
  T x, y, z;
  inline Point3DTemplate() {
  }
  template<typename TP> inline Point3DTemplate(const Point3DTemplate<TP> &src)
    : x((T)src.x), y((T)src.y), z((T)src.z) {
  }
  template<typename TS> inline Point3DTemplate(const Size3DTemplate<TS> &s)
    : x((T)s.cx), y((T)s.cy), z((T)s.cz)
  {
  }
  inline Point3DTemplate(const T &_x, const T &_y, const T &_z)
    : x(_x), y(_y), z(_z) {
  }
  inline T length() const {
    return sqrt(x*x + y*y + z*z);
  }

  Point3DTemplate &normalize() {
    const T l = length();
    if(l != 0) {
	    *this /= l;
    }
    return *this;
  }

  template<typename TP> inline Point3DTemplate &operator+=(const Point3DTemplate<TP> &p) {
    x += (T)p.x; y += (T)p.y; z += (T)p.z;
    return *this;
  }
  template<typename TP> inline Point3DTemplate &operator-=(const Point3DTemplate<TP> &p) {
    x -= (T)p.x; y -= (T)p.y; z -= (T)p.z;
    return *this;
  }
  template<typename TP> inline Point3DTemplate operator+(const Point3DTemplate<TP> &p) const {
    return Point3DTemplate(x + (T)p.x, y + (T)p.y, z + (T)p.z);
  }
  template<typename TP> inline Size3DTemplate<T> operator-(const Point3DTemplate<TP> &p) const {
    return Size3DTemplate<T>(x - (T)p.x, y - (T)p.y, z - (T)p.z);
  }
  template<typename TP> inline T operator*(const Point3DTemplate<TP> &p) const {
    return x * (T)p.x + y * (T)p.y + z * (T)p.z;
  }
  template<typename TS> inline Point3DTemplate operator+(const Size3DTemplate<TS> &sz) const {
    return Point3DTemplate(x + (T)sz.cx, y + (T)sz.cy, z + (T)sz.cz);
  }
  template<typename TS> inline Point3DTemplate operator-(const Size3DTemplate<TS> &sz) const {
    return Point3DTemplate(x - (T)sz.cx, y - (T)sz.cy, z - (T)sz.cz);
  }
  template<typename TS> inline Point3DTemplate &operator+=(const Size3DTemplate<TS> &sz) {
    x += (T)sz.cx; y += (T)sz.cy; z += (T)sz.cz;  return *this;
  }
  template<typename TS> inline Point3DTemplate &operator-=(const Size3DTemplate<TS> &sz) {
    x -= (T)sz.cx; y -= (T)sz.cy; z -= (T)sz.cz;  return *this;
  }
  template<typename TS> inline Point3DTemplate operator*(const TS &d) const {
    const T factor = (T)d;
    return Point3DTemplate(x * factor, y * factor, z * factor);
  }
  template<typename TS> inline Point3DTemplate operator/(const TS &d) const {
    const T factor = (T)d;
    return Point3DTemplate(x / factor, y / factor, z / factor);
  }
  template<typename TS> inline Point3DTemplate &operator*=(const TS &d) {
    const T factor = (T)d;
    x *= factor; y *= factor; z *= factor;
    return *this;
  }
  template<typename TS> inline Point3DTemplate &operator/=(const TS &d) {
    const T factor = (T)d;
    x /= factor; y /= factor; z /= factor;
    return *this;
  }
  inline Point3DTemplate operator-() const {
    return Point3DTemplate(-x, -y, -z);
  }
  template<typename TP> inline T distance(const Point3DTemplate<TP> &p) const {
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

template<typename T> T distance(const Point3DTemplate<T> &p1, const Point3DTemplate<T> &p2) {
  return p1.distance(p2);
}

// angle in radians between p1 and p2
template<typename T> T angle(const Point3DTemplate<T> &p1, const Point3DTemplate<T> &p2) {
  const T l1 = p1.length();
  const T l2 = p2.length();
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

template<typename T> Point3DTemplate<T> cross(const Point3DTemplate<T> &a, const Point3DTemplate<T> &b) {
  return Point3DTemplate<T>(a.y*b.z - a.z*b.y
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
