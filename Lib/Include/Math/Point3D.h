#pragma once

#include <math.h>
#include "MathLib.h"
#include "Double80.h"

// A point in 3-dimensional space
template<typename T> class Point3DTemplate {
public:
  T x, y, z;
  inline Point3DTemplate() {
  }
  inline Point3DTemplate(const T &_x, const T &_y, const T &_z) : x(_x), y(_y), z(_z) {
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

  inline Point3DTemplate &operator+=(const Point3DTemplate &p) {
    x += p.x; y += p.y; z += p.z; return *this;
  }
  inline Point3DTemplate &operator-=(const Point3DTemplate &p) {
    x -= p.x; y -= p.y; z -= p.z; return *this;
  }
  inline T operator*(const Point3DTemplate &p) const {
    return x*p.x + y*p.y + z*p.z;
  };
  inline Point3DTemplate &operator*=(const T &d) {
    x *= d; y *= d; z *= d; return *this;
  }
  inline Point3DTemplate &operator/=(const T &d) {
    x /= d; y /= d; z /= d; return *this;
  }
  inline Point3DTemplate operator+(const Point3DTemplate &p) const {
    return Point3DTemplate(x + p.x, y + p.y, z + p.z);
  }
  inline Point3DTemplate operator-(const Point3DTemplate &p) const {
    return Point3DTemplate(x - p.x, y - p.y, z - p.z);
  }
  inline Point3DTemplate operator*(const T &d) const {
    return Point3DTemplate(x * d, y * d, z * d);
  }
  inline Point3DTemplate operator/(const T &d) const {
    return Point3DTemplate(x / d, y / d, z / d);
  }
  inline Point3DTemplate operator-() const {
    return Point3DTemplate(-x, -y, -z);
  }
  inline T distance(const Point3DTemplate &p) const {
    return sqrt(sqr(x-p.x)+sqr(y-p.y)+sqr(z-p.z));
  }
  inline bool operator==(const Point3DTemplate &p) const {
    return (x==p.x) && (y==p.y) && (z==p.z);
  }
  inline bool operator!=(const Point3DTemplate &p) const {
    return (x != p.x) || (y != p.y) || (z != p.z);
  }
  inline bool operator<(const Point3DTemplate &p) const {
    return (x < p.x) && (y < p.y) && (z < p.z);
  }
  inline bool operator<=(const Point3DTemplate &p) const {
    return (x <= p.x) && (y <= p.y) && (z <= p.z);
  }

  inline String toString(int precision = 3) const {
    return format(_T("(%s,%s,%s)")
                 ,::toString(x, precision).cstr()
                 ,::toString(y, precision).cstr()
                 ,::toString(z, precision).cstr()
                 );
  }
};

template<typename T> T distance(const Point3DTemplate<T> &p1, const Point3DTemplate<T> &p2) {
  return sqrt(sqr(p1.x - p2.x) + sqr(p1.y - p2.y) + sqr(p1.z - p2.z));
}

// angle in radians between p1 and p2
template<typename T> T angle(const Point3DTemplate<T> &p1, const Point3DTemplate<T> &p2) {
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
//      debugLog("l1:%le, l2:%le, p1:%s, p2:%s\n", l1, l2, p1.toString(10).cstr(), p2.toString(10).cstr());
      return acos(f);
    }
  }
}

template<typename T> Point3DTemplate<T> crossProduct(const Point3DTemplate<T> &a, const Point3DTemplate<T> &b) {
  return Point3DTemplate<T>(a.y*b.z - a.z*b.y
                           ,a.z*b.x - a.x*b.z
                           ,a.x*b.y - a.y*b.x
                           );
}

typedef Point3DTemplate<float   > FloatPoint3D;
typedef Point3DTemplate<double  > Point3D;
typedef Point3DTemplate<Double80> D80Point3D;
typedef Point3DTemplate<Real    > RealPoint3D;

typedef FunctionTemplate<Point3D, Point3D> Point3DFunction;
typedef FunctionTemplate<Point3D, Real>    Function3D;
