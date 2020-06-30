#include "pch.h"
#include <D3DGraphics/D3Math.h>

// https://gamedev.stackexchange.com/questions/28395/rotating-vector3-by-a-quaternion
D3DXVECTOR3 rotate(const D3DXVECTOR3 &v, const D3DXQUATERNION &q) {
  // Extract the vector part of the quaternion
  const D3DXVECTOR3 u(q.x, q.y, q.z);

  // Extract the scalar part of the quaternion
  const float s = q.w;

  // Do the math
  return (2.0f * (u*v)) * u
       + (s*s  -  u*u ) * v
       + (2.0f *  s   ) * cross(u, v);
}

D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir, int unitIndex) {
  return createRotation(E[unitIndex], dir);
}

#if defined(_DEBUG)

#if defined(verify)
#undef verify
#endif
#define verify(exp) (void)((exp) || (showError(_T("%s line %d:%s"), __TFUNCTION__, __LINE__, _T(#exp)), 0))
#endif

D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  const D3DXVECTOR3    unitDir = unitVector(dir), unitUp = unitVector(up);
  const float          s = unitDir * unitUp;
  const float          p = 1 - fabs(s);
  verify(p > 1e-5);
  const D3DXVECTOR3    uup     = unitVector(unitUp - (unitDir*unitUp) * unitDir);  // unitDir,uup are ortonormal, span(dir,up) = span(unitDir,uup)
  const D3DXQUATERNION q1      = createOrientation(unitDir);
  const D3DXQUATERNION q2      = createRotation(rotate(E[2], q1), uup);
  const D3DXQUATERNION result  = q1*q2;
  return result;
}

D3DXQUATERNION createRotation(const D3DXVECTOR3 &from, const D3DXVECTOR3 &to) {
  verify(length(from) != 0);
  verify(length(to  ) != 0);
  const float    phi = angle(from, to);
  D3DXQUATERNION q;
  if(phi == 0) {
    return *D3DXQuaternionIdentity(&q);
  } else {
    return createRotation(cross(from, to),phi);
  }
}

D3DXQUATERNION createRotation(const D3DXVECTOR3 &axis, float rad) {
  D3DXQUATERNION q;
  return *D3DXQuaternionRotationAxis(&q, &axis, rad);
}

void getDirUp(const D3DXQUATERNION &q, D3DXVECTOR3 &dir, D3DXVECTOR3 &up) {
  dir = rotate(E[0],q);
  up  = rotate(E[2],q);
}
