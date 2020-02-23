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

#ifdef _DEBUG

#ifdef verify
#undef verify
#endif
#define verify(exp) (void)((exp) || (showError(_T("%s line %d:%s"), __TFUNCTION__, __LINE__, _T(#exp)), 0))
#endif

D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  const D3DXVECTOR3    udir    = unitVector(dir);
  const D3DXQUATERNION q1      = createOrientation(udir);
  const D3DXVECTOR3    uup     = unitVector(up - (udir*up) * udir);  // uup and udir are ortonormal, uupp.length=1, span(dir,up) = span(udir,uup)
  const D3DXQUATERNION q2      = createRotation(rotate(E[2], q1), uup);
  const D3DXQUATERNION result  = q1*q2;
/*
  D3DXVECTOR3 testDir, testUp;
  getDirUp(result, testDir, testUp);
  float v1 = fabs(angle(testDir, udir));
  float v2 = fabs(angle(testUp , uup ));
  if((v1 > 1e-6) || (v2 > 1e-6)) {
    D3DXVECTOR3 td, tu;
    getDirUp(result, td, tu);
    float          diruup     = dir  * uup;                      // expected 0
    float          udiruup    = udir * uup;                      // expected 0
    verify(fabs(diruup ) <= 1e-6f);
    verify(fabs(udiruup) <= 1e-6f);
    D3DXVECTOR3    right1      = unitVector(cross(dir , up)); 
    D3DXVECTOR3    right2      = unitVector(cross(udir, uup));   // right2 == right1
    verify(length(right1 - right2) <= 1e-6);
    D3DXVECTOR3    rotE0q1     = rotate(E[0], q1    );           // expected udir
    D3DXVECTOR3    rotE0result = rotate(E[0], result);           // expected udir
    D3DXVECTOR3    rotE2result = rotate(E[2], result);           // expected uup
    int fisk = 1;
  }
*/
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
  dir = rotate(E[0],q);
  up  = rotate(E[2],q);
}
