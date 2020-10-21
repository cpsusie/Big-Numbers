#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectAnimatedVisual.h>
#include <D3DGraphics/D3Scene.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

D3Scene::D3Scene(bool rightHanded)
  : m_device(nullptr)
  , m_rightHanded(rightHanded)
  , m_ambientColor(0)
  , m_lightsEnabled(10)
  , m_lightsDefined(10)
{
  m_undefinedMaterial.setUndefined();
}

D3Scene::~D3Scene() {
  close();
}

void D3Scene::initDevice(HWND hwnd) {
  m_device = new D3Device(hwnd); TRACE_NEW(m_device);
  const D3DCAPS &deviceCaps = m_device->getDeviceCaps();
  m_ambientColor      = m_device->getAmbientColor();
  m_lightsEnabled.setCapacity(getMaxLightCount()).clear();
  m_lightsDefined.setCapacity(getMaxLightCount()).clear();
  addLight(   D3Light::createDefaultLight());
  addMaterial(D3Material::createDefaultMaterial());
}

void D3Scene::close() {
  PropertyContainer::clear();
  destroyAllLightControls();
  removeAllVisuals();
  removeAllCameras();
  m_materialMap.clear();
  m_lightsEnabled.clear();
  m_lightsDefined.clear();
  SAFEDELETE(m_device);
}

LPDIRECT3DDEVICE D3Scene::getDirectDevice() const {
  return getDevice().getDirectDevice();
}

D3Camera &D3Scene::addCamera(HWND hwnd, D3Camera *src) {
  const UINT oldCount = getCameraCount();
  D3Camera *camera = (src == nullptr) ? new D3Camera(*this, hwnd) : src->clone(hwnd); TRACE_NEW(camera);
  m_cameraArray.add(camera);
  const UINT newCount = getCameraCount();
  notifyPropertyChanged(SP_CAMERACOUNT, &oldCount, &newCount);
  return *camera;
}

void D3Scene::removeCamera(D3Camera &camera) {
  const intptr_t index = m_cameraArray.getFirstIndex(&camera);
  if(index >= 0) {
    const UINT oldCount = getCameraCount();
    D3Camera *camera = m_cameraArray[index];
    m_cameraArray.remove(index);
    SAFEDELETE(camera);
    const UINT newCount = getCameraCount();
    notifyPropertyChanged(SP_CAMERACOUNT, &oldCount, &newCount);
  }
}

void D3Scene::removeAllCameras() {
  const UINT oldCount = getCameraCount();
  if(oldCount > 0) {
    const bool notifyEnable = setNotifyEnable(false);
    while(getCameraCount() > 0) {
      D3Camera *camera = m_cameraArray.last();
      removeCamera(*camera);
    }
    setNotifyEnable(notifyEnable);
    const UINT newCount = getCameraCount();
    notifyPropertyChanged(SP_CAMERACOUNT, &oldCount, &newCount);
  }
}

void D3Scene::setRightHanded(bool rightHanded) {
  const bool oldValue = getRightHanded();
  if(rightHanded != oldValue) {
    const bool notifyEnable = setNotifyEnable(false);
    m_rightHanded = rightHanded;
    for(D3Camera *cam : m_cameraArray) {
      cam->setRightHanded(rightHanded);
    }
    setNotifyEnable(notifyEnable);
    notifyPropertyChanged(SP_RIGHTHANDED, &oldValue, &rightHanded);
  }
}

UINT D3Scene::getTextureCoordCount() const {
  return getDevice().getTextureCoordCount();
}

void D3Scene::render(const D3Camera &camera) {
  D3Device &device = getDevice();
  device.setAmbientColor(m_ambientColor).beginRender(camera);
  try {
    for(D3SceneObjectVisual *obj : m_visualArray) {
      if(obj->isVisible()) {
        obj->draw();
      }
    }
    device.endRender();
  } catch(...) {
    device.endRender();
    throw;
  }
}

void D3Scene::render(CameraSet cameraSet) {
  cameraSet &= getActiveCameraSet();
  for(Iterator<UINT> it = cameraSet.getIterator(); it.hasNext();) {
    render(*m_cameraArray[it.next()]);
  }
}

void D3Scene::addVisual(D3SceneObjectVisual *obj) {
  const UINT oldCount = (UINT)m_visualArray.size();
  m_visualArray.add(obj);
  notifyVisualCountChanged(oldCount);
}

void D3Scene::removeVisual(D3SceneObjectVisual *obj) {
  const intptr_t index = m_visualArray.getFirstIndex(obj);
  if(index >= 0) {
    removeVisual(index);
  }
}

void D3Scene::removeVisual(size_t index) {
  D3SceneObjectVisual *obj = m_visualArray[index];
  if(obj->getType() == SOTYPE_ANIMATEDOBJECT) {
    ((D3SceneObjectAnimatedVisual*)obj)->stopAnimation();
  }
  const UINT oldCount = (UINT)m_visualArray.size();
  m_visualArray.remove(index);
  notifyVisualCountChanged(oldCount);
}

void D3Scene::removeAllVisuals() {
  while(getVisualCount() > 0) {
    removeVisual(getVisualCount()-1);
  }
}

bool D3Scene::isVisual(const D3SceneObjectVisual *obj) const {
  return m_visualArray.getFirstIndex((D3SceneObjectVisual*)obj) >= 0;
}

void D3Scene::stopAllAnimations() {
  for(D3VisualIterator it = getVisualIterator(OBJMASK_ANIMATEDOBJECT); it.hasNext();) {
    ((D3SceneObjectAnimatedVisual*)it.next())->stopAnimation();
  }
}

void D3Scene::notifyVisualCountChanged(UINT oldCount) {
  const UINT newCount = (UINT)m_visualArray.size();
  setProperty(SP_VISUALCOUNT, oldCount, newCount);
}

void D3Scene::setAnimationFrameIndex(int &oldValue, int newValue) {
  setProperty(SP_ANIMATIONFRAMEINDEX, oldValue, newValue);
}
