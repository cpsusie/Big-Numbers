#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>

D3Camera::D3Camera(D3Scene &scene, HWND hwnd)
  : D3SceneObject( scene )
  , m_hwnd(  hwnd  )
  , m_rightHanded(true)
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
  LPDIRECT3DDEVICE      device  = getDirectDevice();
  D3DPRESENT_PARAMETERS present = D3DeviceFactory::getDefaultPresentParameters(getHwnd());
  if(present.BackBufferWidth && present.BackBufferHeight) {
    V(device->ResetEx(&present, NULL));
    setProjMatrix();
  }
}

bool D3Camera::ptInRect(CPoint p) const {
  CRect r;
  GetWindowRect(getHwnd(), &r);
  return r.PtInRect(p);
}

D3Camera &D3Camera::setProjMatrix() {
  D3DXMATRIX m;
  setProperty(CAM_PROJECTION, m_projMatrix, createProjMatrix(m));
  return *this;
}

// notify listeners with properyId=CAM_VIEW
D3Camera &D3Camera::setViewMatrix() {
  D3DXMATRIX m;
  setProperty(CAM_VIEW, m_viewMatrix, createViewMatrix(m));
  return *this;
}

D3DXMATRIX &D3Camera::createProjMatrix(D3DXMATRIX &m) const {
  const CSize size = getWinSize();
  D3DXMatrixPerspectiveFov(m, m_viewAngel, (float)size.cx / size.cy, m_nearViewPlane, 200.0f, getRightHanded());
  return m;
}

D3DXMATRIX &D3Camera::createViewMatrix(D3DXMATRIX &m) const {
  return m_world.createViewMatrix(m, getRightHanded());
}

D3Camera &D3Camera::setViewAngel(float angel) {
  if(angel > 0 && angel < D3DX_PI) {
    m_viewAngel = angel;
    setProjMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setNearViewPlane(float zn) {
  if(zn > 0) {
    m_nearViewPlane = zn;
    setProjMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setRightHanded(bool rightHanded) {
  if(rightHanded != m_rightHanded) {
    m_rightHanded = rightHanded;
    setProjMatrix();
    setViewMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setWorld(const D3World &world) {
  m_world = world;
  m_world.setScaleAll(1);
  return setViewMatrix();
}

D3Camera &D3Camera::resetPos() {
  return setLookAt(D3DXVECTOR3(0, -5, 0), D3DXORIGIN, D3DXVECTOR3(0, 0, 1));
}

D3Camera &D3Camera::setPos(const D3DXVECTOR3 &pos) {
  m_world.setPos(pos);
  return setViewMatrix();
}

D3Camera &D3Camera::setOrientation(const D3DXQUATERNION &q) {
  m_world.setOrientation(q);
  return setViewMatrix();
}

D3Camera &D3Camera::setLookAt(const D3DXVECTOR3 &point) {
  return setLookAt(getPos(), point, getUp());
}

D3Camera &D3Camera::setLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  return setWorld(D3World().setPos(pos).setOrientation(createOrientation(lookAt - pos, up)));
}

D3Ray D3Camera::getPickedRay(const CPoint &point) const {
  const CSize winSize = getWinSize();

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * point.x) / winSize.cx) - 1) / m_projMatrix._11 * m_nearViewPlane;
  v.y = -(((2.0f * point.y) / winSize.cy) - 1) / m_projMatrix._22 * m_nearViewPlane;
  v.z = -m_nearViewPlane;

  const D3DXMATRIX m = invers(m_viewMatrix);
  return D3Ray(m*v, v*m);
}

D3SceneObjectVisual *D3Camera::getPickedVisual(const CPoint &p, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  D3Ray tmpRay, &pickedRay = ray ? *ray : tmpRay;
  pickedRay = getPickedRay(p);
  return getScene().getPickedVisual(pickedRay, mask, hitPoint, dist, info);
}

void D3Camera::render() {
  getScene().render(*this);
}

String D3Camera::toString() const {
  const CSize size = getWinSize();
  return format(_T("View angel:%.1lf, Near view:%.3lf, winSize:(%3d,%3d)")
               , degrees(getViewAngel())
               , getNearViewPlane()
               , size.cx,size.cy
               );
}
