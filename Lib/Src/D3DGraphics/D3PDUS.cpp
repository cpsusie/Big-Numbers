#include "pch.h"

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
  return format(_T("P:%s\nD:%s\nU:%s\nR:%s\nS:%s\nView:\n%sWorld:\n%s")
               ,::toString(m_pos).cstr()
               ,::toString(m_dir).cstr()
               ,::toString(m_up).cstr()
               ,::toString(getRight()).cstr()
               ,::toString(m_scale).cstr()
               ,indentString(::toString(getViewMatrix()),2).cstr()
               ,indentString(::toString(getWorldMatrix()),2).cstr()
               );
}

void D3PosDirUpScale::dumpWorld() {
  debugLog(_T("%sWorld:\n%s_________________________\n"), toString().cstr(), ::toString(getWorldMatrix()).cstr());
}
