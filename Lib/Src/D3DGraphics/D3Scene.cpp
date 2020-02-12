#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3LightControl.h>

int D3Scene::s_textureCoordCount;

#pragma warning(disable : 4073)
#pragma init_seg(lib)


D3Scene::D3Scene(bool rightHanded)
  : m_stateFlags(0)
  , m_device(NULL)
  , m_camPDUS(rightHanded)
  , m_defaultObjPDUS(rightHanded)
  , m_origoPDUS(rightHanded)
  , m_lightsEnabled(10)
  , m_lightsDefined(10)
{
  if(rightHanded) {
    setFlags(SC_RIGHTHANDED);
  }
  m_undefinedMaterial.setUndefined();
}

D3Scene::~D3Scene() {
  close();
}

void D3Scene::notifyPropertyChanged(int id, const void *oldValue, const void *newValue) {
  if(isSet(SC_INITDONE) && isSet(SC_PROPCHANGES)) {
    __super::notifyPropertyChanged(id, oldValue, newValue);
  }
}

void D3Scene::init(HWND hwnd) {
  if(isSet(SC_INITDONE)) {
    throwException(_T("%s:Scene already initialized"), __TFUNCTION__);
  }
  m_hwnd = hwnd;

  m_device = D3DeviceFactory::createDevice(m_hwnd);

  D3DCAPS deviceCaps;
  V(m_device->GetDeviceCaps(&deviceCaps));

  s_textureCoordCount = deviceCaps.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK;
  m_maxLightCount     = deviceCaps.MaxActiveLights;
  m_lightsEnabled.setCapacity(m_maxLightCount); m_lightsEnabled.clear();
  m_lightsDefined.setCapacity(m_maxLightCount); m_lightsDefined.clear();

  addLight(     LIGHT::createDefaultLight());
  addMaterial(  MATERIAL::createDefaultMaterial());

  initTrans();
  m_renderState.getValuesFromDevice(m_device);
  m_renderState.setDefault();
  m_renderState.setValuesToDevice(m_device);
  setFlags(SC_INITDONE | SC_PROPCHANGES);
}

void D3Scene::close() {
  if(!isSet(SC_INITDONE)) return;
  PropertyContainer::clear();
  destroyAllLightControls();
  removeAllSceneObjects();
  m_materialMap.clear();
  m_lightsEnabled.clear();
  m_lightsDefined.clear();
  SAFERELEASE(m_device);
  clrFlags(SC_INITDONE);
}

void D3Scene::initTrans() {
  m_viewAngel      = radians(45);
  m_nearViewPlane  = 0.1f;
  updateDevProjMatrix();
  resetCamTrans();
  resetDefaultObjTrans();
}

D3Scene &D3Scene::selectMaterial(UINT materialId) {
  if(materialId != m_renderState.m_selectedMaterialId) {
    const MATERIAL &mat = getMaterial(materialId);
    if(!mat.isDefined()) {
      showWarning(_T("Material with id=%u is undefined"), materialId);
      return *this;
    }
    FV(m_device->SetMaterial(&mat));
    if(mat.getOpacity() < 1.0f) {
      setCullMode(D3DCULL_CCW)
     .setZEnable(D3DZB_FALSE)
     .setAlphaBlendEnable(true)
     .setSrcBlend(D3DBLEND_SRCALPHA)
     .setDstBlend(D3DBLEND_INVSRCALPHA);
    } else {
      setCullMode(D3DCULL_CCW)
     .setZEnable(D3DZB_TRUE)
     .setAlphaBlendEnable(false);
    }
    m_renderState.m_selectedMaterialId = materialId;
  }
  return *this;
}

void D3Scene::setRightHanded(bool rightHanded) {
  const bool oldValue = getRightHanded();
  if(rightHanded != oldValue) {
    clrFlags(SC_PROPCHANGES);
    if(rightHanded) {
      setFlags(SC_RIGHTHANDED);
    } else {
      clrFlags(SC_RIGHTHANDED);
    }
    m_camPDUS.setRightHanded(       rightHanded);
    m_defaultObjPDUS.setRightHanded(rightHanded);
    m_origoPDUS.setRightHanded(     rightHanded);
    updateDevProjMatrix();
    setFlags(SC_PROPCHANGES);
    notifyPropertyChanged(SP_RIGHTHANDED, &oldValue, &rightHanded);
  }
}

void D3Scene::setCamPDUS(const D3PosDirUpScale &pdus) {
  setProperty(SP_CAMERAPDUS, m_camPDUS, pdus);
}

void D3Scene::setCamPos(const D3DXVECTOR3 &pos) {
  setCamPDUS(D3PosDirUpScale(m_camPDUS).setPos(pos));
}

void D3Scene::setCamOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  setCamPDUS(D3PosDirUpScale(m_camPDUS).setOrientation(dir, up));
}

void D3Scene::setCamLookAt(const D3DXVECTOR3 &point) {
  setCamLookAt(getCamPos(), point, getCamUp());
}

void D3Scene::setCamLookAt(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  setCamPDUS(D3PosDirUpScale(m_camPDUS).setPos(pos).setOrientation(lookAt - pos, up).resetScale());
}

void D3Scene::resetCamTrans() {
  setCamLookAt(D3DXVECTOR3(0, -5, 0), D3DXORIGIN, D3DXVECTOR3(0, 0, 1));
}

void D3Scene::setDefaultObjTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up, const D3DXVECTOR3 &scale) {
  m_defaultObjPDUS.setPos(pos).setOrientation(dir, up).setScale(scale);
}

void D3Scene::resetDefaultObjTrans() {
  m_defaultObjPDUS.resetPos().resetOrientation().resetScale();
}

D3Scene &D3Scene::updateDevViewMatrix() {
  return setDevViewMatrix(m_camPDUS.getViewMatrix());
}

D3Scene &D3Scene::setViewAngel(float angel) {
  if(angel > 0 && angel < D3DX_PI) {
    m_viewAngel = angel;
    updateDevProjMatrix();
  }
  return *this;
}

D3Scene &D3Scene::setNearViewPlane(float zn) {
  if(zn > 0) {
    m_nearViewPlane = zn;
    updateDevProjMatrix();
  }
  return *this;
}

String D3Scene::getCamString() const {
  return format(_T("Camera:View angel:%.1lf, Near view:%.3lf\n%s")
               ,degrees(getViewAngel())
               ,getNearViewPlane()
               ,m_camPDUS.toString().cstr()
               );
}

D3Scene &D3Scene::updateDevProjMatrix() {
  const CSize size = getClientRect(getHwnd()).Size();
  D3DXMATRIX matProj;
  return setDevProjMatrix(D3DXMatrixPerspectiveFov(matProj, m_viewAngel, (float)size.cx/size.cy, m_nearViewPlane, 200.0f, getRightHanded()));
}

D3Scene &D3Scene::setDevProjMatrix(const D3DXMATRIX &m) {
  const D3DXMATRIX currentProj = getDevProjMatrix();
  setDevTransformation(D3DTS_PROJECTION, m);
  if(m != currentProj) {
    notifyPropertyChanged(SP_PROJECTIONTRANSFORMATION, &currentProj, &m);
  }
  return *this;
}

D3Scene &D3Scene::setDevTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m) {
  V(m_device->SetTransform(id, &m));
  return *this;
}

D3DXMATRIX D3Scene::getDevTransformation(D3DTRANSFORMSTATETYPE id) const {
  D3DXMATRIX m;
  V(m_device->GetTransform(id, &m));
  return m;
}

// -------------------------------- LIGHT ------------------------------------
BitSet D3Scene::getLightControlsVisible() const {
  BitSet result(m_maxLightCount);
  for(Iterator<D3SceneObject*> it = getObjectIterator(OBJMASK_LIGHTCONTROL); it.hasNext();) {
    D3LightControl *lc = (D3LightControl*)it.next();
    if(lc->isVisible()) {
      result.add(lc->getLightIndex());
    }
  }
  return result;
}

D3LightControl *D3Scene::setLightControlVisible(UINT lightIndex, bool visible) {
  if(!isLightDefined(lightIndex)) {
    showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__, lightIndex);
    return NULL;
  }
  D3LightControl *lc = findLightControlByLightIndex(lightIndex);
  if(lc == NULL) {
    lc = addLightControl(lightIndex);
  }
  if(lc) {
    lc->setVisible(visible);
  }
  return lc;
}

D3LightControl *D3Scene::addLightControl(UINT lightIndex) {
  if(!isLightDefined(lightIndex)) {
    showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__, lightIndex);
    return NULL;
  }
  D3LightControl *result = findLightControlByLightIndex(lightIndex);
  if(result != NULL) return result;
  LIGHT param = getLight(lightIndex);
  switch(param.Type) {
  case D3DLIGHT_DIRECTIONAL    : result = new D3LightControlDirectional(*this, lightIndex); TRACE_NEW(result); break;
  case D3DLIGHT_POINT          : result = new D3LightControlPoint(      *this, lightIndex); TRACE_NEW(result); break;
  case D3DLIGHT_SPOT           : result = new D3LightControlSpot(       *this, lightIndex); TRACE_NEW(result); break;
  default                      : throwException(_T("Unknown lighttype for light %d:%d"), lightIndex, param.Type);
  }
  addSceneObject(result);
  return result;
}

void D3Scene::destroyLightControl(UINT lightIndex) {
  D3LightControl *lc = findLightControlByLightIndex(lightIndex);
  if(lc == NULL) return;
  removeSceneObject(lc);
  SAFEDELETE(lc);
}

void D3Scene::destroyAllLightControls() {
  const LightArray la = getAllLights();
  for(size_t i = 0; i < la.size(); i++) {
    const LIGHT &l = la[i];
    destroyLightControl(l.getIndex());
  }
}

UINT D3Scene::addLight(const D3DLIGHT &light) {
  const UINT oldCount = getLightCount();
  const UINT index    = getFirstFreeLightIndex();
  V(m_device->SetLight(   index, &light));
  V(m_device->LightEnable(index, TRUE  ));
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
  V(m_device->LightEnable(lightIndex, FALSE));
  const UINT newCount = oldCount - 1;
  notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
}

void D3Scene::removeAllLights() {
  BitSet allLights = getLightsDefined();
  clrFlags(SC_PROPCHANGES);
  for(Iterator<size_t> it = allLights.getIterator(); it.hasNext();) {
    removeLight((UINT)it.next());
  }
  setFlags(SC_PROPCHANGES);
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

void D3Scene::setLight(const LIGHT &param) {
  const int index = param.getIndex();
  if(!isLightDefined(index)) {
    showWarning(_T("%s:Light %d is undefined"),__TFUNCTION__, index);
    return;
  }
  const LIGHT oldLp = getLight(index);
  if(param == oldLp) return;
  V(m_device->SetLight(index, &param));
  V(m_device->LightEnable(index, param.isEnabled()?TRUE:FALSE));
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
  for(Iterator<D3SceneObject*> it = getObjectIterator(OBJMASK_LIGHTCONTROL); it.hasNext();) {
    D3LightControl *lc = (D3LightControl*)it.next();
    if(lc->getLightIndex() == lightIndex) {
      return lc;
    }
  }
  return NULL;
}

LIGHT D3Scene::getLight(UINT lightIndex) const {
  if(!isLightDefined(lightIndex)) {
    return LIGHT(0).setUndefined();
  } else {
    LIGHT lp(lightIndex);
    V(m_device->GetLight(lightIndex, &lp));
    return lp.setEnabled(isLightEnabled(lightIndex));
  }
}

LightArray D3Scene::getAllLights() const {
  BitSet lightSet = getLightsDefined();
  LightArray result(lightSet.size());
  for(Iterator<size_t> it = lightSet.getIterator(); it.hasNext();) {
    result.add(getLight((UINT)it.next()));
  }
  return result;
}

int D3Scene::getFirstFreeLightIndex() const {
  for(int i = 0; i < m_maxLightCount; i++) {
    if(!isLightDefined(i)) {
      return i;
    }
  }
  throwException(_T("No free lights available. Max=%d"), getMaxLightCount());
  return -1;
}

String D3Scene::getLightString(UINT lightIndex) const {
  return isLightDefined(lightIndex)
       ? getLight(lightIndex).toString()
       : format(_T("Light[%d]:Undefined"), lightIndex);
}

String D3Scene::getLightString() const {
  String result;
  BitSet lightSet = getLightsDefined();
  for(Iterator<size_t> it = lightSet.getIterator(); it.hasNext(); ) {
    const UINT lightIndex = (UINT)it.next();
    if(result.length()) result += _T("\n");
    result += getLightString(lightIndex);
  }
  return result;
}

// ---------------------------- MATERIAL -----------------------------

UINT D3Scene::addMaterial(const D3DMATERIAL &material) {
  const UINT oldCount = getMaterialCount();
  const UINT id       = getFirstFreeMaterialId();
  MATERIAL  m(id);
  m = material;
  m_materialMap.put(id, m);
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
  return id;
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
  const int newCount = oldCount-1;
  m_materialMap.remove(materialId);
  unselectMaterial();
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

void D3Scene::setMaterial(const MATERIAL &material) {
  if(!material.isDefined()) {
    addMaterial(material);
  } else {
    const UINT id = material.getId();
    if(!isMaterialDefined(id)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("id=%u, material undefined"), id);
    }
    unselectMaterial();
    MATERIAL *m = m_materialMap.get(id);
    setProperty(SP_MATERIALPARAMETERS, *m, material);
  }
}

void D3Scene::setLightControlMaterial(const MATERIAL &lcMaterial) {
  assert(lcMaterial.isDefined());
  MATERIAL *m = m_materialMap.get(lcMaterial.getId());
  *m = lcMaterial;
}

String D3Scene::getMaterialString(UINT materialId) const {
  return getMaterial(materialId).toString();
}

String D3Scene::getMaterialString() const {
  Array<MATERIAL> matArray(getMaterialCount());
  for(Iterator<Entry<CompactUIntKeyType, MATERIAL> > it = m_materialMap.getEntryIterator(); it.hasNext();) {
    Entry<CompactUIntKeyType, MATERIAL> &e = it.next();
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

void D3Scene::render() {
  V(m_device->Clear(0
                   ,NULL
                   ,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
                   ,getBackgroundColor()
                   ,1.0f
                   ,0
                   ));

  updateDevViewMatrix();

  V(m_device->BeginScene());
  for(Iterator<D3SceneObject*> it = getObjectIterator(); it.hasNext();) {
    D3SceneObject *obj = it.next();
    if(obj->isVisible()) {
      obj->getPDUS().setRightHanded(getRightHanded());
      setDevWorldMatrix(obj->getWorldMatrix());
      obj->draw();
    }
  }

  V(m_device->EndScene());
  V(m_device->Present(NULL, NULL, NULL, NULL));

  setProperty(SP_RENDERTIME, m_renderTime, Timestamp());
}

LPDIRECT3DINDEXBUFFER D3Scene::allocateIndexBuffer(bool int32, UINT count, UINT *bufferSize) {
  const int itemSize = int32 ? sizeof(long) : sizeof(short);
  UINT tmp;
  UINT &totalSize    = bufferSize ? *bufferSize : tmp;
  totalSize = itemSize*count;
  LPDIRECT3DINDEXBUFFER result;
  V(m_device->CreateIndexBuffer(totalSize, 0, int32 ? D3DFMT_INDEX32 : D3DFMT_INDEX16, D3DPOOL_DEFAULT, &result, NULL));
  TRACE_CREATE(result);
  return result;
}

LPD3DXMESH D3Scene::allocateMesh(DWORD fvf , UINT faceCount, UINT vertexCount, DWORD options) {
  LPD3DXMESH result;
  FV(D3DXCreateMeshFVF(faceCount, vertexCount, options, fvf, m_device, &result));
  TRACE_CREATE(result);
  return result;
}

LPD3DXEFFECT D3Scene::compileEffect(const String &srcText, StringArray &errorMsg) {
  LPD3DXEFFECT effect         = NULL;
  LPD3DXBUFFER compilerErrors = NULL;
  DWORD        flags          = D3DXFX_NOT_CLONEABLE;
#ifdef D3DXFX_LARGEADDRESS_HANDLE
  flags |= D3DXFX_LARGEADDRESSAWARE;
#endif
  USES_CONVERSION;
  const char *text = T2A(srcText.cstr());
  int textLen = (int)strlen(text);
  try {
    V(D3DXCreateEffect(m_device, text, textLen, NULL, NULL, flags, NULL, &effect, &compilerErrors));
    TRACE_CREATE(effect);
    return effect;
  } catch(Exception e) {
    const String errorText = (char*)compilerErrors->GetBufferPointer();
    errorMsg = StringArray(Tokenizer(errorText, _T("\n\r")));
    return NULL;
  }
}

void D3Scene::OnSize() {
  if(m_device) {
    D3DPRESENT_PARAMETERS present = D3DeviceFactory::getDefaultPresentParameters(getHwnd());
    if(present.BackBufferWidth && present.BackBufferHeight) {
      V(m_device->ResetEx(&present, NULL));
      updateDevProjMatrix();
    }
  }
}

void D3Scene::addSceneObject(D3SceneObject *obj) {
  const UINT oldCount = (UINT)m_objectArray.size();
  m_objectArray.add(obj);
  notifyObjectCountChanged(oldCount);
}

void D3Scene::removeSceneObject(D3SceneObject *obj) {
  const intptr_t index = m_objectArray.getFirstIndex(obj);
  if(index >= 0) {
    removeSceneObject(index);
  }
}

void D3Scene::removeSceneObject(size_t index) {
  D3SceneObject *obj = m_objectArray[index];
  if(obj->getType() == SOTYPE_ANIMATEDOBJECT) {
    ((D3AnimatedSurface*)obj)->stopAnimation();
  }
  const UINT oldCount = (UINT)m_objectArray.size();
  m_objectArray.remove(index);
  notifyObjectCountChanged(oldCount);
}

void D3Scene::removeAllSceneObjects() {
  while(getObjectCount() > 0) {
    removeSceneObject(getObjectCount()-1);
  }
}

bool D3Scene::isSceneObject(const D3SceneObject *obj) const {
  return m_objectArray.getFirstIndex((D3SceneObject*)obj) >= 0;
}

void D3Scene::stopAllAnimations() {
  for(Iterator<D3SceneObject*> it = getObjectIterator(OBJMASK_ANIMATEDOBJECT); it.hasNext();) {
    ((D3AnimatedSurface*)it.next())->stopAnimation();
  }
}

void D3Scene::notifyObjectCountChanged(UINT oldCount) {
  const UINT newCount = (UINT)m_objectArray.size();
  setProperty(SP_OBJECTCOUNT, oldCount, newCount);
}

void D3Scene::setAnimationFrameIndex(int &oldValue, int newValue) {
  setProperty(SP_ANIMATIONFRAMEINDEX, oldValue, newValue);
}
