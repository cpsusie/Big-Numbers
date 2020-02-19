#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>

D3Camera::D3Camera(D3Scene &scene, HWND hwnd)
  : m_scene( scene )
  , m_hwnd(  hwnd  )
  , m_pdus(  true  )
{
  m_viewAngel       = radians(45);
  m_nearViewPlane   = 0.1f;
  m_backgroundColor = D3DCOLOR_XRGB(192, 192, 192);
  createProjMatrix(m_projMatrix);
  resetPos();
}

D3Camera::~D3Camera() {
  m_hwnd = (HWND)INVALID_HANDLE_VALUE;
}

void D3Camera::OnSize() {
  LPDIRECT3DDEVICE device = m_scene.getDirectDevice();
  D3DPRESENT_PARAMETERS present = D3DeviceFactory::getDefaultPresentParameters(getHwnd());
  if(present.BackBufferWidth && present.BackBufferHeight) {
    V(device->ResetEx(&present, NULL));
    ajourProjMatrix();
  }
}

bool D3Camera::ptInRect(CPoint p) const {
  CRect r;
  GetWindowRect(getHwnd(), &r);
  return r.PtInRect(p);
}

void D3Camera::ajourProjMatrix() {
  D3DXMATRIX m;
  setProperty(CAM_PROJECTION, m_projMatrix, createProjMatrix(m));
}

D3DXMATRIX &D3Camera::createProjMatrix(D3DXMATRIX &m) const {
  const CSize size = getWinSize();
  D3DXMatrixPerspectiveFov(m, m_viewAngel, (float)size.cx / size.cy, m_nearViewPlane, 200.0f, getRightHanded());
  return m;
}

D3Camera &D3Camera::setViewAngel(float angel) {
  if(angel > 0 && angel < D3DX_PI) {
    m_viewAngel = angel;
    ajourProjMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setNearViewPlane(float zn) {
  if(zn > 0) {
    m_nearViewPlane = zn;
    ajourProjMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setRightHanded(bool rightHanded) {
  return setPDUS(D3PosDirUpScale(m_pdus).setRightHanded(rightHanded));
}

D3Camera &D3Camera::resetPos() {
  return setLookAt(D3DXVECTOR3(0, -5, 0), D3DXORIGIN, D3DXVECTOR3(0, 0, 1));
}

D3Camera &D3Camera::setPDUS(const D3PosDirUpScale &pdus) {
  setProperty(CAM_PDUS, m_pdus, pdus);
  return *this;
}

D3Camera &D3Camera::setPos(const D3DXVECTOR3 &pos) {
  return setPDUS(D3PosDirUpScale(m_pdus).setPos(pos));
}
D3Camera &D3Camera::setOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  return setPDUS(D3PosDirUpScale(m_pdus).setOrientation(dir, up));
}

D3Camera &D3Camera::setLookAt(const D3DXVECTOR3 &point) {
  return setLookAt(getPos(), point, getUp());
}

D3Camera &D3Camera::setLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  return setPDUS(D3PosDirUpScale(m_pdus).setPos(pos).setOrientation(lookAt - pos, up).resetScale());
}

D3Ray D3Camera::getPickedRay(const CPoint &point) const {
  const CSize winSize = getWinSize();

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * point.x) / winSize.cx) - 1) / m_projMatrix._11 * m_nearViewPlane;
  v.y = -(((2.0f * point.y) / winSize.cy) - 1) / m_projMatrix._22 * m_nearViewPlane;
  v.z = -m_nearViewPlane;

  const D3DXMATRIX world = m_pdus.getWorldMatrix();
  return D3Ray(world*v, v*world);
}

D3SceneObject *D3Camera::getPickedObject(const CPoint &p, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  D3Ray tmpRay, &pickedRay = ray ? *ray : tmpRay;
  pickedRay = getPickedRay(p);
  return m_scene.getPickedObject(pickedRay, mask, hitPoint, dist, info);
}

void D3Camera::render() {
  m_scene.render(*this);
}

String D3Camera::toString() const {
  const CSize size = getWinSize();
  return format(_T("View angel:%.1lf, Near view:%.3lf, winSize:(%3d,%3d)")
               , degrees(getViewAngel())
               , getNearViewPlane()
               , size.cx,size.cy
               );
}
