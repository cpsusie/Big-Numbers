#include "pch.h"

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

D3DXMATRIX createRotationMatrix(const D3DXVECTOR3 &axes, double rad) {
  D3DXMATRIX matRot;
  return *D3DXMatrixRotationAxis(&matRot, &axes, (float)rad);
}

D3DXVECTOR3 rotate(const D3DXVECTOR3 &v, const D3DXVECTOR3 &axes, double rad) {
  D3DXMATRIX matRot;
  return *D3DXMatrixRotationAxis(&matRot, &axes, (float)rad) * v;
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

// ---------------------------------------------------------------------------------

D3PosDirUpScale::D3PosDirUpScale() {
  m_pos   = D3DXVECTOR3(0,0,0);
  m_scale = D3DXVECTOR3(1,1,1);
  m_dir   = D3DXVECTOR3(0,1,0);
  m_up    = D3DXVECTOR3(0,0,1);
  updateView();
}

void D3PosDirUpScale::setPos(const D3DXVECTOR3 &pos) {
  m_pos = pos;
  updateView();
}

void D3PosDirUpScale::setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  m_dir = unitVector(dir);
  m_up  = unitVector(up);
  updateView();
}

void D3PosDirUpScale::setScale(const D3DXVECTOR3 &scale) {
  m_scale = scale;
}

D3DXMATRIX D3PosDirUpScale::getRotationMatrix() const {
  D3PosDirUpScale tmp(*this);
  tmp.setPos(  D3DXVECTOR3(0,0,0));
  tmp.setScale(D3DXVECTOR3(1,1,1));
  return tmp.getWorldMatrix();
}

D3DXMATRIX D3PosDirUpScale::getScaleMatrix() const {
  D3DXMATRIX scale;
  return *D3DXMatrixScaling(&scale, m_scale.x, m_scale.y, m_scale.z);
}

D3DXMATRIX D3PosDirUpScale::getWorldMatrix() const {
  D3DXMATRIX invView = invers(m_view);
  D3DXMATRIX scale;
  return *D3DXMatrixScaling(&scale, m_scale.x, m_scale.y, m_scale.z) * invView;
}

D3DXMATRIX D3PosDirUpScale::getViewMatrix() const {
  return m_view;
}

void D3PosDirUpScale::setWorldMatrix(const D3DXMATRIX &world) {
  m_pos   = D3DXVECTOR3( world._41,  world._42,  world._43);
  m_dir   = D3DXVECTOR3(-world._31, -world._32, -world._33);
  m_up    = D3DXVECTOR3( world._21,  world._22,  world._23);
  const float sx = length(D3DXVECTOR3(world._11,world._12,world._13));
  const float sy = length(D3DXVECTOR3(world._21,world._22,world._23));
  const float sz = length(D3DXVECTOR3(world._31,world._32,world._33));
  m_scale = D3DXVECTOR3(sx,sy,sz);
  m_dir /= sz;
  m_up  /= sy;
  updateView();
}

void D3PosDirUpScale::updateView() {
  D3DXVECTOR3 lookAt = m_pos + m_dir;
  D3DXMatrixLookAt(&m_view, &m_pos, &lookAt, &m_up);
//  dumpWorld();
}

String D3PosDirUpScale::toString() const {
  return format(_T("P:%s D:%s U:%s R:%s S:%s")
               ,::toString(m_pos).cstr()
               ,::toString(m_dir).cstr()
               ,::toString(m_up).cstr()
               ,::toString(getRight()).cstr()
               ,::toString(m_scale).cstr());
}

void D3PosDirUpScale::dumpWorld() {
  debugLog(_T("%sWorld:\n%s_________________________\n"), toString().cstr(), ::toString(getWorldMatrix()).cstr());
}
