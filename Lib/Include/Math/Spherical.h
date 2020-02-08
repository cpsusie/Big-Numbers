#pragma once

#include <MathUtil.h>
#include "Point3D.h"

template<typename T> class SphericalTemplate {
protected:
  void init(const T &x, const T &y, const T &z) {
    const T dxy2 = x*x + y*y;
    if(y >= 0) {
      if(dxy2 > 0) {
        r     = (T)sqrt(dxy2 + z*z);
        fi    = (T)acos(z / r);
        theta = (T)(acos(x / sqrt(dxy2)));
      } else {
        r     = z;
        fi    = theta = 0;
      }
    } else { // y < 0
      r       = (T)sqrt(dxy2 + z*z);
      fi      = (T)acos(z / r);
      theta   = (T)(2*M_PI - acos(x / sqrt(dxy2)));
    }
  }
public:
  T      r;     // Distance to (0,0,0), 0 <= r
  T      fi;    // Angle between z-axis and (x,y,z), 0 <= fi <= pi
  T      theta; // Angle between x-axis and (x,y,z), 0 <= theta <= 2pi
                // theta > pi => y < 0
  SphericalTemplate() : r(0), fi(0), theta(0) {
  }
  template<typename TP> SphericalTemplate(const Point3DTemplate<TP> &p) {
    init((T)p.x, (T)p.y, (T)p.z);
  }
  template<typename X, typename Y, typename Z> SphericalTemplate(const X &x, const Y &y, const Z &z) {
    init((T)x, (T)y, (T)z);
  }
  operator Point3DTemplate<T>() const {
    Point3DTemplate<T> p;
    T                  tmp;
    p.x = theta;
    p.z = fi;
    sincos(p.x,p.y);
    sincos(p.z,tmp);
    tmp *= r;   // r * sin(fi);

    p.x *= tmp; // r * sin(fi) * cos(theta);
    p.y *= tmp; // r * sin(fi) * sin(theta);
    p.z *= r;   // r * cos(fi);
    return p;
  }
  String toString(int precision = 1, bool rad = false) const {
    if(rad) {
      return format(_T("(%s,%s,%s)")
                   ,::toString(r    , precision).cstr()
                   ,::toString(fi   , precision).cstr()
                   ,::toString(theta, precision).cstr()
                   );
    } else {
      return format(_T("(%s,%s,%s)")
                   ,::toString(r              , precision).cstr()
                   ,::toString(RAD2GRAD(fi)   , precision).cstr()
                   ,::toString(RAD2GRAD(theta), precision).cstr()
                   );
    }
  }
};

typedef SphericalTemplate<float   > FloatSpherical;
typedef SphericalTemplate<double  > Spherical;
typedef SphericalTemplate<Double80> D80Spherical;
typedef SphericalTemplate<Real    > RealSpherical;
