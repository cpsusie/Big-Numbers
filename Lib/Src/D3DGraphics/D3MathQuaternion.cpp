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

/* Old, slow way
  D3DXMATRIX m;
  D3DXVECTOR3 r2 = *D3DXMatrixRotationQuaternion(&m, &q) * v;
  return r2;
*/


D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir, int unitIndex) {
  return createRotation(createUnitVector(unitIndex), dir);
}

D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  D3DXQUATERNION q1        = createOrientation(dir);
  D3DXVECTOR3    up1       = rotate(createUnitVector(2),q1);
  D3DXVECTOR3    upProjDir = ((dir*up) / (dir*dir)) * dir; // up projected to dir
  D3DXVECTOR3    realUp    = unitVector(up - upProjDir);   // realUp and dir are ortogonal, realUp.length=1, span(dir,up) = span(dir,realUp)
  D3DXQUATERNION q2        = createRotation(up1, realUp);
  D3DXQUATERNION result    = q2 * q1;
  D3DXVECTOR3 testDir, testUp;
  getDirUp(result, testDir, testUp);
  if((fabs(angle(testDir, dir)) > 1e-6) || (fabs(angle(testUp, realUp)) > 1e-6)) {
    int fisk = 1;
  }
  return result;
}

D3DXQUATERNION createRotation(const D3DXVECTOR3 &from, const D3DXVECTOR3 &to) {
  const float    phi = angle(from, to);
  D3DXQUATERNION q;
  if(phi == 0) {
    return *D3DXQuaternionIdentity(&q);
  } else {
    return createRotation(cross(from, to),phi);
  }
}

D3DXQUATERNION createRotation(const D3DXVECTOR3 &axis, float angle) {
  D3DXQUATERNION q;
  return *D3DXQuaternionRotationAxis(&q, &axis, angle);
}

void getDirUp(const D3DXQUATERNION &q, D3DXVECTOR3 &dir, D3DXVECTOR3 &up) {
  dir = rotate(createUnitVector(0),q);
  up  = rotate(createUnitVector(2),q);
}
