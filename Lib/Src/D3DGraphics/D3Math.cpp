#include "pch.h"
#include <MFCUtil/ColorSpace.h>

D3DXMATRIX  createIdentityMatrix() {
  D3DXMATRIX m;
  return *D3DXMatrixIdentity(&m);
}

D3DXMATRIX  createTranslateMatrix(const D3DXVECTOR3 &v) {
  D3DXMATRIX m;
  return *D3DXMatrixTranslation(&m,v.x,v.y,v.z);
}

D3DXMATRIX createScaleMatrix(const D3DXVECTOR3 &s) {
  D3DXMATRIX m;
  return *D3DXMatrixScaling(&m, s.x, s.y, s.z);
}

D3DXMATRIX createRotationMatrix(const D3DXVECTOR3 &axes, float rad) {
  D3DXMATRIX matRot;
  return *D3DXMatrixRotationAxis(&matRot, &axes, rad);
}

D3DXQUATERNION createOrientation(const D3DXVECTOR3 &dir) {
  D3DXQUATERNION    q;
  const D3DXVECTOR3 E1 = createUnitVector(0);
  float             phi = angle(E1, dir);
  if(phi == 0) {
    return *D3DXQuaternionIdentity(&q);
  } else {
    const D3DXVECTOR3 axis = crossProduct(E1, dir);
    return *D3DXQuaternionRotationAxis(&q, &axis, phi);
  }
}

D3DXQUATERNION createRotation(const D3DXVECTOR3 &axis, float angle) {
  D3DXQUATERNION q;
  return *D3DXQuaternionRotationAxis(&q, &axis, angle);
}

D3DXVECTOR3 rotate(const D3DXVECTOR3 &v, const D3DXVECTOR3 &axes, float rad) {
  D3DXMATRIX matRot;
  return *D3DXMatrixRotationAxis(&matRot, &axes, rad) * v;
}

D3DXVECTOR3 operator*(const D3DXQUATERNION &q, const D3DXVECTOR3 &v) {
  D3DXMATRIX m;
  return *D3DXMatrixRotationQuaternion(&m, &q) * v;
}

D3DXMATRIX &D3DXMatrixPerspectiveFov(D3DXMATRIX &mat, FLOAT angel, FLOAT apsect, FLOAT zn, FLOAT fn, bool rightHanded) {
  return rightHanded
       ? *D3DXMatrixPerspectiveFovRH(&mat, angel, apsect, zn, fn)
       : *D3DXMatrixPerspectiveFovLH(&mat, angel, apsect, zn, fn);
}

D3DXMATRIX &D3DXMatrixLookAt(D3DXMATRIX &view, const D3DXVECTOR3 &eye, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up, bool rightHanded) {
  return rightHanded
       ? *D3DXMatrixLookAtRH(&view, &eye, &lookAt, &up)
       : *D3DXMatrixLookAtLH(&view, &eye, &lookAt, &up);
}

static D3DXVECTOR3 randomUnitVector() {
  D3DXVECTOR3 v;
  v.x = (float)rand() / RAND_MAX;
  v.y = (float)rand() / RAND_MAX;
  v.z = (float)rand() / RAND_MAX;
  return unitVector(v);
}

float length(const D3DXVECTOR3 &v) {
  return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float operator*(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  return v1.x*v2.x + v1.y*v2.y+v1.z*v2.z;
}

float angle(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  return acosf((v1*v2) / (length(v1)*length(v2)));
}

D3DXVECTOR3 crossProduct(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  D3DXVECTOR3 result;
  return *D3DXVec3Cross(&result, &v1, &v2);
}

D3DXVECTOR3 unitVector(const D3DXVECTOR3 &v) {
  D3DXVECTOR3 tmp;
  return *D3DXVec3Normalize(&tmp, &v);
}

D3DXVECTOR3 createUnitVector(int i) {
  switch(i) {
  case 0 : return D3DXVECTOR3(1,0,0);
  case 1 : return D3DXVECTOR3(0,1,0);
  case 2 : return D3DXVECTOR3(0,0,1);
  default: throwInvalidArgumentException(__TFUNCTION__, _T("i=%d. Must be [0..2]"), i);
           return D3DXVECTOR3(0,0,0);
  }
}

D3DXVECTOR3 ortonormalVector(const D3DXVECTOR3 &v) {
  const D3DXVECTOR3 tmp = unitVector(v);
  for(;;) {
    D3DXVECTOR3 Y = randomUnitVector();
    Y -= tmp * (Y * tmp); // make Y perpendicular to X
    const float l = length(Y);
    if(l != 0) {
      return Y / l;
    }
  }
}

D3DXMATRIX transpose(const D3DXMATRIX &m) {
  D3DXMATRIX  result;
  return *D3DXMatrixTranspose(&result, &m);
}

D3DXMATRIX invers(const D3DXMATRIX &m) {
  D3DXMATRIX  result;
  return *D3DXMatrixInverse(&result, NULL, &m);
}

float det(const D3DXMATRIX &m) {
  return D3DXMatrixDeterminant(&m);
}

/*
D3DXVECTOR3 myMultiply(const D3DXMATRIX &m, const D3DXVECTOR3 &v) {
  D3DXVECTOR3 r;
  r.x = m._11 * v.x + m._12 * v.y + m._13 * v.z;
  r.y = m._21 * v.x + m._22 * v.y + m._23 * v.z;
  r.z = m._31 * v.x + m._32 * v.y + m._33 * v.z;
  return r;
}
*/

D3DXVECTOR3 operator*(const D3DXMATRIX &m, const D3DXVECTOR3 &v) {
  D3DXVECTOR4 v4;
  D3DXVec3Transform(&v4, &v, &m);
  return D3DXVECTOR3(v4.x,v4.y,v4.z);

//  D3DXVECTOR3 result1 = myMultiply(m,v);
}

D3DXVECTOR3 operator*(const D3DXVECTOR3 &v, const D3DXMATRIX &m) {
  D3DXVECTOR3 result;
  result.x = v.x * m._11 + v.y * m._21 + v.z * m._31;
  result.y = v.x * m._12 + v.y * m._22 + v.z * m._32;
  result.z = v.x * m._13 + v.y * m._23 + v.z * m._33;
  return result;
}

String toString(const D3DXVECTOR3 &v, int dec) {
  const int n = dec+3;
  return format(_T("(% *.*f,% *.*f,% *.*f)"), n,dec,v.x, n,dec,v.y, n,dec,v.z);
}

String toString(const D3DXVECTOR4 &v, int dec) {
  const int n = dec+3;
  return format(_T("(% *.*f,% *.*f,% *.*f,% *.*f)"), n,dec,v.x, n,dec,v.y, n,dec,v.z, n,dec,v.w);
}

String toString(const D3DXQUATERNION &q, int dec) {
  const int n = dec + 3;
  return format(_T("(% *.*f,% *.*f,% *.*f,% *.*f)")
               ,n, dec, q.x, n, dec, q.y, n, dec, q.z, n, dec, q.w);
}

String toString(const D3DXMATRIX &m, int dec) {
  const int n = dec+3;
  String result;
  for(int r = 0; r < 4; r++) {
    for(int c = 0; c < 4; c++) {
      if(c) result += _T(",");
      result += format(_T("% *.*f"), n,dec,m.m[r][c]);
    }
    result += _T("\n");
  }
  return result;
}


