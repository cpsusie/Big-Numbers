#include "pch.h"

D3Spherical::operator D3DXVECTOR3() const {
  D3DXVECTOR3 v;
  const double tmp = r * sin(fi);

  v.x = (float)(tmp * cos(theta));
  v.y = (float)(tmp * sin(theta));
  v.z = (float)(r * cos(fi));
  return v;
}
