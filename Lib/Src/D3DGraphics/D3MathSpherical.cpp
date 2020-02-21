#include "pch.h"
#include <D3DGraphics/D3Spherical.h>

D3Spherical::operator D3DXVECTOR3() const {
  const float tmp = r * sin(fi);
  return D3DXVECTOR3(tmp * cos(theta), tmp * sin(theta), r * cos(fi));
}
