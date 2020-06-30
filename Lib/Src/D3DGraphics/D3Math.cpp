#include "pch.h"
#include <MFCUtil/ColorSpace.h>

D3DXMATRIX createIdentityMatrix() {
  D3DXMATRIX m;
  return *D3DXMatrixIdentity(&m);
}

D3DXMATRIX createTranslateMatrix(const D3DXVECTOR3 &v) {
  D3DXMATRIX m;
  return *D3DXMatrixTranslation(&m,v.x,v.y,v.z);
}

D3DXVECTOR3 randUnitVector(RandomGenerator &rnd) {
  D3DXVECTOR3 v(rnd.nextFloat(-1, 1), rnd.nextFloat(-1, 1), rnd.nextFloat(-1, 1));
  return unitVector(v);
}

float length(const D3DXVECTOR3 &v) {
  return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}

float operator*(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  return v1.x*v2.x + v1.y*v2.y+v1.z*v2.z;
}

float angle(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  const float l1 = length(v1);
  const float l2 = length(v2);
  if((l1 == 0) || (l2 == 0)) {
    return 0.0f;
  } else {
    const float f = (v1 * v2) / (l1 * l2);
    if(f <= -1) {
      return D3DX_PI;
    } else if(f >= 1) {
      return 0.0f;
    } else {
      return acosf(f);
    }
  }
}

D3DXVECTOR3 cross(const D3DXVECTOR3 &v1, const D3DXVECTOR3 &v2) {
  D3DXVECTOR3 result;
  return *D3DXVec3Cross(&result, &v1, &v2);
}

D3DXVECTOR3 unitVector(const D3DXVECTOR3 &v) {
  D3DXVECTOR3 tmp;
  return *D3DXVec3Normalize(&tmp, &v);
}

D3DXVECTOR3 E[3] = {
  D3DXVECTOR3(1,0,0)
 ,D3DXVECTOR3(0,1,0)
 ,D3DXVECTOR3(0,0,1)
};

D3DXVECTOR3 createUnitVector(UINT i) {
  if(i < 3) {
    return E[i];
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("i=%u. Must be [0..2]"), i);
  return D3DXORIGIN;
}

D3DXVECTOR3 ortonormalVector(const D3DXVECTOR3 &v) {
  const D3DXVECTOR3 tmp = unitVector(v);
  for(;;) {
    D3DXVECTOR3 Y = randUnitVector();
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

D3DXMATRIX inverse(const D3DXMATRIX &m) {
  D3DXMATRIX  result;
  return *D3DXMatrixInverse(&result, NULL, &m);
}

float det(const D3DXMATRIX &m) {
  return D3DXMatrixDeterminant(&m);
}

D3DXVECTOR3 operator*(const D3DXMATRIX &m, const D3DXVECTOR3 &v) {
  D3DXVECTOR4 v4;
  D3DXVec3Transform(&v4, &v, &m);
  return D3DXVECTOR3(v4.x,v4.y,v4.z);
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
