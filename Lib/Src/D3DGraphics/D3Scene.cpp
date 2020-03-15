#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>

#pragma warning(disable : 4073)
#pragma init_seg(lib)

D3Scene::D3Scene(bool rightHanded)
  : m_device(NULL)
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
  m_lightsEnabled.setCapacity(getMaxLightCount()); m_lightsEnabled.clear();
  m_lightsDefined.setCapacity(getMaxLightCount()); m_lightsDefined.clear();
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
  return getDevice();
}

D3Camera &D3Scene::addCamera(HWND hwnd, D3Camera *src) {
  const UINT oldCount = getCameraCount();
  D3Camera *camera = (src == NULL) ? new D3Camera(*this, hwnd) : src->clone(hwnd);
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
    for(Iterator<D3Camera*> it = m_cameraArray.getIterator(); it.hasNext();) {
      it.next()->setRightHanded(rightHanded);
    }
    setNotifyEnable(notifyEnable);
    notifyPropertyChanged(SP_RIGHTHANDED, &oldValue, &rightHanded);
  }
}

// -------------------------------- D3Light ------------------------------------

D3LightControl *D3Scene::addLightControl(UINT lightIndex) {
  if(!isLightDefined(lightIndex)) {
    showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__, lightIndex);
    return NULL;
  }
  D3LightControl *result = findLightControlByLightIndex(lightIndex);
  if(result != NULL) return result;
  const D3Light &param = getLight(lightIndex);
  switch(param.Type) {
  case D3DLIGHT_DIRECTIONAL    : result = new D3LightControlDirectional(*this, lightIndex); TRACE_NEW(result); break;
  case D3DLIGHT_POINT          : result = new D3LightControlPoint(      *this, lightIndex); TRACE_NEW(result); break;
  case D3DLIGHT_SPOT           : result = new D3LightControlSpot(       *this, lightIndex); TRACE_NEW(result); break;
  default                      : throwException(_T("Unknown lighttype for light %u:%d"), lightIndex, param.Type);
  }
  addVisual(result);
  return result;
}

void D3Scene::destroyLightControl(UINT lightIndex) {
  D3LightControl *lc = findLightControlByLightIndex(lightIndex);
  if(lc == NULL) return;
  removeVisual(lc);
  SAFEDELETE(lc);
}

void D3Scene::destroyAllLightControls() {
  LightArray la = getAllLights();
  for(Iterator<D3Light> it = la.getIterator(); it.hasNext();) {
    destroyLightControl(it.next().getIndex());
  }
}

UINT D3Scene::addLight(const D3DLIGHT &light) {
  const UINT oldCount = getLightCount();
  const UINT index    = getFirstFreeLightIndex();
  getDevice().setLight(index, light).enableLight(index, true);
  m_lightsDefined.add(index);
  m_lightsEnabled.add(index);
  const UINT newCount = oldCount + 1;
  notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
  return index;
}

void D3Scene::removeLight(UINT lightIndex) {
  if(!isLightDefined(lightIndex)) return;
  if(isLightEnabled(lightIndex)) {
    m_lightsEnabled.remove(lightIndex);
  }
  destroyLightControl(lightIndex);
  const UINT oldCount = getLightCount();
  m_lightsDefined.remove(lightIndex);
  getDevice().enableLight(lightIndex, FALSE);
  const UINT newCount = oldCount - 1;
  notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
}

void D3Scene::removeAllLights() {
  BitSet allLights = getLightsDefined();
  const bool notifyEnable = setNotifyEnable(false);
  for(Iterator<size_t> it = allLights.getIterator(); it.hasNext();) {
    removeLight((UINT)it.next());
  }
  setNotifyEnable(notifyEnable);
}

UINT D3Scene::getMaxLightCount() const {
  return getDevice().getMaxLightCount();
}

void D3Scene::setLightEnabled(UINT lightIndex, bool enabled) {
  if(!isLightDefined(lightIndex)) return;
  setLight(getLight(lightIndex).setEnabled(enabled));
}

void D3Scene::setLightDirection(UINT lightIndex, const D3DXVECTOR3 &dir) {
  if(!isLightDefined(lightIndex)) return;
  setLight(getLight(lightIndex).setDirection(dir));
}

void D3Scene::setLightPosition(UINT lightIndex, const D3DXVECTOR3 &pos) {
  if(!isLightDefined(lightIndex)) return;
  setLight(getLight(lightIndex).setPosition(pos));
}

void D3Scene::setLight(const D3Light &param) {
  const int index = param.getIndex();
  if(!isLightDefined(index)) {
    showWarning(_T("%s:Light %d is undefined"),__TFUNCTION__, index);
    return;
  }
  const D3Light oldLp = getLight(index);
  if(param == oldLp) return;
  getDevice().setLight(param);
  if(param.isEnabled() != oldLp.isEnabled()) {
    if(param.isEnabled()) {
      m_lightsEnabled.add(index);
    } else {
      m_lightsEnabled.remove(index);
    }
  }
  notifyPropertyChanged(SP_LIGHTPARAMETERS, &oldLp, &param);
}

D3LightControl *D3Scene::findLightControlByLightIndex(int lightIndex) {
  for(D3VisualIterator it = getVisualIterator(OBJMASK_LIGHTCONTROL); it.hasNext();) {
    D3LightControl *lc = (D3LightControl*)it.next();
    if(lc->getLightIndex() == lightIndex) {
      return lc;
    }
  }
  return NULL;
}

D3Light D3Scene::getLight(UINT lightIndex) const {
  if(!isLightDefined(lightIndex)) {
    return D3Light(0).setUndefined();
  } else {
    D3Light light(lightIndex);
    light = getDevice().getLight(lightIndex);
    return light.setEnabled(isLightEnabled(lightIndex));
  }
}

LightArray D3Scene::getAllLights() const {
  BitSet lightSet = getLightsDefined();
  LightArray result(lightSet.size());
  for(Iterator<size_t> it = lightSet.getIterator(); it.hasNext();) {
    const UINT lightIndex = (UINT)it.next();
    result.add(getLight(lightIndex));
  }
  return result;
}

UINT D3Scene::getFirstFreeLightIndex() const {
  const UINT maxLightCount = getMaxLightCount();
  for(UINT i = 0; i < maxLightCount; i++) {
    if(!isLightDefined(i)) {
      return i;
    }
  }
  throwException(_T("No free lights available. Max=%u"), maxLightCount);
  return -1;
}

String D3Scene::getLightString(UINT lightIndex) const {
  return isLightDefined(lightIndex)
       ? getLight(lightIndex).toString()
       : format(_T("Light[%d]:Undefined"), lightIndex);
}

String D3Scene::getLightString() const {
  String result;
  LightArray allLights = getAllLights();
  for(Iterator<D3Light> it = allLights.getIterator(); it.hasNext(); ) {
    const D3Light &light = it.next();
    if(result.length()) result += _T("\n");
    result += light.toString();
  }
  return result;
}

void D3Scene::setSpecularEnable(bool enabled) {
  getDevice().setSpecularEnable(enabled);
}
bool D3Scene::getSpecularEnable() const {
  return getDevice().getSpecularEnable();
}

// ---------------------------- D3Material -----------------------------

UINT D3Scene::addMaterial(const D3DMATERIAL &material) {
  const UINT oldCount = getMaterialCount();
  const UINT id       = getFirstFreeMaterialId();
  D3Material  m(id);
  m = material;
  m_materialMap.put(id, m);
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
  return id;
}

UINT D3Scene::addMaterialWithColor(D3DCOLOR color) {
  return addMaterial(D3Material::createMaterialWithColor(color));
}

UINT D3Scene::getFirstFreeMaterialId() const {
  for(UINT id = 0;; id++) {
    if(m_materialMap.get(id) == NULL) {
      return id;
    }
  }
}

void D3Scene::removeMaterial(UINT materialId) {
  if((materialId == 0) || (!isMaterialDefined(materialId))) { // cannot remove material 0
    return;
  }
  const int oldCount = getMaterialCount();
  m_materialMap.remove(materialId);
  const int newCount = getMaterialCount();
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

void D3Scene::setMaterial(const D3Material &material) {
  if(!material.isDefined()) {
    addMaterial(material);
  } else {
    const UINT id = material.getId();
    if(!isMaterialDefined(id)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("id=%u, material undefined"), id);
    }
    D3Material *m = m_materialMap.get(id);
    setProperty(SP_MATERIALPARAMETERS, *m, material);
  }
}

void D3Scene::setLightControlMaterial(const D3Material &lcMaterial) {
  assert(lcMaterial.isDefined());
  D3Material *m = m_materialMap.get(lcMaterial.getId());
  *m = lcMaterial;
}

String D3Scene::getMaterialString(UINT materialId) const {
  return getMaterial(materialId).toString();
}

String D3Scene::getMaterialString() const {
  Array<D3Material> matArray(getMaterialCount());
  for(Iterator<Entry<CompactUIntKeyType, D3Material> > it = m_materialMap.getEntryIterator(); it.hasNext();) {
    Entry<CompactUIntKeyType, D3Material> &e = it.next();
    matArray.add(e.getValue());
  }
  matArray.sort(materialCmp);
  const size_t n = matArray.size();
  String result;
  for(size_t i = 0; i < n; i++) {
    if(i > 0) result += _T("\n");
    result += matArray[i].toString();
  }
  return result;
}

UINT D3Scene::getTextureCoordCount() const {
  return getDevice().getTextureCoordCount();
}

void D3Scene::render(const D3Camera &camera) {
  D3Device &device = getDevice();
  device.setAmbientColor(m_ambientColor).beginRender(camera);
  for(D3VisualIterator it = getVisualIterator(); it.hasNext();) {
    D3SceneObjectVisual *obj = it.next();
    if(obj->isVisible()) {
      obj->draw();
    }
  }
  device.endRender();
}

void D3Scene::render(CameraSet cameraSet) {
  cameraSet &= getCameraArray().getActiveCameraSet();
  for(Iterator<UINT> it = cameraSet.getIterator(); it.hasNext();) {
    render(*m_cameraArray[it.next()]);
  }
}

LPDIRECT3DINDEXBUFFER D3Scene::allocateIndexBuffer(bool int32, UINT count, UINT *bufferSize) {
  const int itemSize = int32 ? sizeof(long) : sizeof(short);
  UINT tmp;
  UINT &totalSize    = bufferSize ? *bufferSize : tmp;
  totalSize = itemSize*count;
  LPDIRECT3DINDEXBUFFER result;
  V(getDirectDevice()->CreateIndexBuffer(totalSize, 0, int32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16, D3DPOOL_DEFAULT, &result, NULL));
  TRACE_CREATE(result);
  return result;
}

LPD3DXMESH D3Scene::allocateMesh(DWORD fvf , UINT faceCount, UINT vertexCount, DWORD options) {
  LPD3DXMESH result;
  V(D3DXCreateMeshFVF(faceCount, vertexCount, options, fvf, getDevice(), &result));
  TRACE_CREATE(result);
  return result;
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
    ((D3SceneObjectAnimatedMesh*)obj)->stopAnimation();
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
    ((D3SceneObjectAnimatedMesh*)it.next())->stopAnimation();
  }
}

void D3Scene::notifyVisualCountChanged(UINT oldCount) {
  const UINT newCount = (UINT)m_visualArray.size();
  setProperty(SP_VISUALCOUNT, oldCount, newCount);
}

void D3Scene::setAnimationFrameIndex(int &oldValue, int newValue) {
  setProperty(SP_ANIMATIONFRAMEINDEX, oldValue, newValue);
}
