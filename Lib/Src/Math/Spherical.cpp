#include "pch.h"
#include <Math/Spherical.h>

Spherical::Spherical() {
  r = fi = theta = 0;
}

Spherical::Spherical(const Point3D &p) {
  init(p.x, p.y, p.z);
}

Spherical::Spherical(double x, double y, double z) {
  init(x, y, z);
}

void Spherical::init(double x, double y, double z) {
  const double dxy2 = x*x + y*y;
  if(y >= 0) {
    if(dxy2 > 0) {
      r     = sqrt(dxy2 + z*z);
      fi    = acos(z / r);
      theta = acos(x / sqrt(dxy2));
    } else {
      r     = z;
      fi    = theta = 0;
    }
  } else { // y < 0
    r       = sqrt(dxy2 + z*z);
    fi      = acos(z / r);
    theta   = 2*M_PI - acos(x / sqrt(dxy2)); 
  }
}

Spherical::operator Point3D() const {
  Point3D p;
  const double tmp = r * sin(fi);

  p.x = tmp * cos(theta);
  p.y = tmp * sin(theta);
  p.z = r * cos(fi);
  return p;
}

String Spherical::toString(int dec, bool rad) const {
  static const TCHAR *form = _T("(%.*lf, %.*lf, %.*lf)");
  if(rad) {
    return format(form, dec,r,dec,fi,dec,theta);
  } else {
    return format(form, dec,r,dec,RAD2GRAD(fi),dec,RAD2GRAD(theta));
  }
}
