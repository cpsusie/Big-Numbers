#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3Camera.h>

D3Camera::D3Camera(D3Scene &scene, HWND hwnd)
  : D3SceneObject( scene )
  , m_hwnd(  hwnd  )
  , m_rightHanded(true)
  , m_visibleLightControlSet(scene.getMaxLightCount())
{
  m_backgroundColor = getDefaultBackgroundColor();
  initWorldAndProjection();
}

D3Camera::D3Camera(const D3Camera *src, HWND hwnd)
: D3SceneObject(src->getScene())
, m_hwnd(hwnd)
, m_rightHanded(src->getRightHanded())
, m_visibleLightControlSet(src->getLightControlsVisible())
{
  setNotifyEnable(false);
  m_backgroundColor = src->getBackgroundColor();
  m_viewAngle       = src->getViewAngle();
  m_nearViewPlane   = src->getNearViewPlane();
  m_farViewPlane    = src->getFarViewPlane();
  m_world           = src->m_world;

  setViewMatrix().setProjMatrix();
  setNotifyEnable(true);
}

D3Camera *D3Camera::clone(HWND hwnd) const {
  return new D3Camera(this, hwnd); // no TRACE_NEW here
}

D3Camera::~D3Camera() {
  m_hwnd = (HWND)INVALID_HANDLE_VALUE;
}

D3Camera &D3Camera::initWorldAndProjection() {
  return initWorld().initProjection();
}

D3Camera &D3Camera::initWorld() {
  return resetPos().resetOrientation();
}

D3Camera &D3Camera::initProjection() {
  return resetProjection();
}

void D3Camera::OnSize() {
  LPDIRECT3DDEVICE      device  = getDirectDevice();
  D3DPRESENT_PARAMETERS present = DirectXDeviceFactory::getInstance().getDefaultPresentParameters(getHwnd());
  if(present.BackBufferWidth && present.BackBufferHeight) {
    V(device->ResetEx(&present, NULL));
    setProjMatrix();
  }
}

D3Camera &D3Camera::setHwnd(HWND hwnd) {
  HWND old = m_hwnd;
  m_hwnd = hwnd;
  if(IsWindow(m_hwnd)) {
    const bool notifyEnable = setNotifyEnable(false);
    setProjMatrix();
    setNotifyEnable(notifyEnable);
  }
  notifyPropertyChanged(CAM_WINDOW, &old, &m_hwnd);
  return *this;
}

bool D3Camera::ptInRect(CPoint p) const {
  CRect r;
  GetWindowRect(getHwnd(), &r);
  return r.PtInRect(p);
}

const BitSet &D3Camera::getLightControlsVisible() const {
  m_visibleLightControlSet &= getScene().getLightsDefined();
  return m_visibleLightControlSet;
}

D3Camera &D3Camera::setLightControlsVisible(const BitSet &set) {
  const BitSet oldSet = getLightControlsVisible();
  BitSet newSet = set & getScene().getLightsDefined();
  if(newSet != oldSet) {
    const bool notifyEnable = setNotifyEnable(false);
    BitSet missing = newSet - oldSet;
    if(!missing.isEmpty()) {
      for(Iterator<size_t> it = missing.getIterator(); it.hasNext();) {
        setLightControlVisible((UINT)it.next(), true);
      }
    }
    BitSet hideSet = oldSet - newSet;
    if(!hideSet.isEmpty()) {
      for(Iterator<size_t> it = hideSet.getIterator(); it.hasNext();) {
        setLightControlVisible((UINT)it.next(), false);
      }
    }
    setNotifyEnable(notifyEnable);
    notifyPropertyChanged(CAM_LIGHTCONTROLSVISIBLE, &oldSet, &getLightControlsVisible());
  }
  return *this;
}

D3LightControl *D3Camera::setLightControlVisible(UINT lightIndex, bool visible) {
  D3Scene &scene = getScene();
  if(!scene.isLightDefined(lightIndex)) {
    showWarning(_T("%s:Light %u is undefined"), __TFUNCTION__, lightIndex);
    return NULL;
  }
  D3LightControl *lc = scene.findLightControlByLightIndex(lightIndex);
  if(lc == NULL) {
    lc = scene.addLightControl(lightIndex);
  }
  if(visible != isLightControlVisible(lightIndex)) {
    BitSet newSet = getLightControlsVisible();
    if(visible) {
      newSet.add(lightIndex);
    } else {
      newSet.remove(lightIndex);
    }
    setProperty(CAM_LIGHTCONTROLSVISIBLE, m_visibleLightControlSet, newSet);
  }
  return lc;
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

static D3DXMATRIX &createMatrixPerspectiveFov(D3DXMATRIX &mat, FLOAT angel, FLOAT apsect, FLOAT zn, FLOAT fn, bool rightHanded) {
  return rightHanded
       ? *D3DXMatrixPerspectiveFovRH(&mat, angel, apsect, zn, fn)
       : *D3DXMatrixPerspectiveFovLH(&mat, angel, apsect, zn, fn);
}

D3DXMATRIX &D3Camera::createProjMatrix(D3DXMATRIX &m) const {
  const CSize size = getWinSize();
  createMatrixPerspectiveFov(m, m_viewAngle, (float)size.cx / size.cy, m_nearViewPlane, m_farViewPlane, getRightHanded());
  return m;
}

D3DXMATRIX &D3Camera::createViewMatrix(D3DXMATRIX &m) const {
  return m_world.createViewMatrix(m, getRightHanded());
}

D3Camera &D3Camera::setViewAngle(float angle) {
  if((angle > 0) && (angle < D3DX_PI)) {
    m_viewAngle = angle;
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

D3Camera &D3Camera::setFarViewPlane(float fn) {
  if(fn > 0) {
    m_farViewPlane = fn;
    setProjMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setRightHanded(bool rightHanded) {
  if(rightHanded != m_rightHanded) {
    m_rightHanded = rightHanded;
    setProjMatrix().setViewMatrix();
  }
  return *this;
}

D3Camera &D3Camera::setD3World(const D3World &world) {
  m_world = world;
  m_world.setScaleAll(1);
  return setViewMatrix();
}

D3Camera &D3Camera::resetPos() {
  return setPos(D3DXVECTOR3(0, -5, 0));
}

D3Camera &D3Camera::resetOrientation() {
  return setLookAt(getPos(), D3DXORIGIN, D3DXVECTOR3(0, 0, 1));
}

D3Camera &D3Camera::resetProjection() {
  m_viewAngle     = getDefaultViewAngle();
  m_nearViewPlane = getDefaultNearViewPlane();
  m_farViewPlane  = getDefaultFarViewPlane();
  return setProjMatrix();
}

D3Camera &D3Camera::resetBackgroundColor() {
  return setBackgroundColor(getDefaultBackgroundColor());
}

D3Camera &D3Camera::resetAll() {
  return resetPos().resetOrientation().resetProjection();
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
  return setD3World(D3World().setLookAt(pos, lookAt, up));
}

D3Ray D3Camera::getPickedRay(const CPoint &point) const {
  const CSize winSize = getWinSize();

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * point.x) / winSize.cx) - 1) / m_projMatrix._11 * m_nearViewPlane;
  v.y = -(((2.0f * point.y) / winSize.cy) - 1) / m_projMatrix._22 * m_nearViewPlane;
  v.z = -m_nearViewPlane;

  const D3DXMATRIX m = inverse(m_viewMatrix);
  return D3Ray(m*v, v*m);
}

D3SceneObjectVisual *D3Camera::getPickedVisual(const CPoint &p, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  D3Ray tmpRay, &pickedRay = ray ? *ray : tmpRay;
  pickedRay = getPickedRay(p);
  return getScene().getPickedVisual(*this, pickedRay, mask, hitPoint, dist, info);
}

void D3Camera::doRender() {
  getScene().render(*this);
}

void D3Camera::render() {
  m_renderLock.wait();
  try {
    doRender();
    m_renderLock.notify();
  } catch(...) {
    m_renderLock.notify();
    throw;
  }
}

void D3Camera::beginAnimate() {
  m_renderLock.wait();
  setNotifyEnable(false);
}

void D3Camera::endAnimate() {
  setNotifyEnable(true);
  m_renderLock.notify();
}

String D3Camera::toString() const {
  const CSize size = getWinSize();
  return format(_T("View angle:%.2lf, Near/Far view:(%.3f,%.3f), WinSize:%s, NotifyEnabled:%s, Listeners:%d")
               , degrees(getViewAngle())
               , getNearViewPlane()
               , getFarViewPlane()
               , ::toString(size).cstr()
               , boolToStr(getNotifyEnable())
               , getListenerCount()
               );
}
