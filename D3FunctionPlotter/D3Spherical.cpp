#include "stdafx.h"

D3Spherical::D3Spherical() {
  r = phi = theta = 0;
}

D3Spherical::D3Spherical(const D3DXVECTOR3 &v) {
  init(v.x, v.y, v.z);
}

D3Spherical::D3Spherical(double x, double y, double z) {
  init(x, y, z);
}

void D3Spherical::init(double x, double y, double z) {
  const double dxy2 = x*x + y*y;
  if(y >= 0) {
    if(dxy2 > 0) {
      r     = sqrt(dxy2 + z*z);
      phi   = acos(z / r);
      theta = acos(x / sqrt(dxy2));
    } else {
      r     = z;
      phi   = theta = 0;
    }
  } else { // y < 0
    r       = sqrt(dxy2 + z*z);
    phi     = acos(z / r);
    theta   = 2*M_PI - acos(x / sqrt(dxy2)); 
  }
}

D3Spherical::operator D3DXVECTOR3() const {
  D3DXVECTOR3 v;
  const double tmp = r * sin(phi);

  v.x = (float)(tmp * cos(theta));
  v.y = (float)(tmp * sin(theta));
  v.z = (float)(r * cos(phi));
  return v;
}

String D3Spherical::toString(int dec, bool rad) const {
  static const TCHAR *form = _T("(%.*lf, %.*lf, %.*lf)");
  if(rad) {
    return format(form, dec,r,dec,phi,dec,theta);
  } else {
    return format(form, dec,r,dec,RAD2GRAD(phi),dec,RAD2GRAD(theta));
  }
}

