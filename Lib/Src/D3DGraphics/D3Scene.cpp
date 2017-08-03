#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3LightControl.h>

DECLARE_THISFILE;

int D3Scene::s_textureCoordCount;

#pragma warning(disable : 4073)
#pragma init_seg(lib)

const D3PosDirUpScale D3Scene::s_pdusOrigo;

D3Scene::D3Scene() {
  m_device            = NULL;
  m_oldObjectCount    = 0;
  m_lightsEnabled     = NULL;
  m_lightsDefined     = NULL;
  m_initDone          = false;
}

D3Scene::~D3Scene() {
  close();
}

void D3Scene::notifyPropertyChanged(int id, const void *oldValue, const void *newValue) {
  if(m_initDone) {
    PropertyContainer::notifyPropertyChanged(id, oldValue, newValue);
  }
}

void D3Scene::init(HWND hwnd) {
  if(m_initDone) {
    throwException(_T("%s:Scene already initialized"), __TFUNCTION__);
  }
  m_hwnd = hwnd;

  m_device = D3DeviceFactory::createDevice(m_hwnd);

  D3DCAPS deviceCaps;
  V(m_device->GetDeviceCaps(&deviceCaps));

  s_textureCoordCount = deviceCaps.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK;
  m_maxLightCount     = deviceCaps.MaxActiveLights;
  m_lightsEnabled     = new BitSet(m_maxLightCount); TRACE_NEW(m_lightsEnabled);
  m_lightsDefined     = new BitSet(m_maxLightCount); TRACE_NEW(m_lightsDefined);

  addLight(     getDefaultLight());
  addMaterial(  getDefaultMaterial());

  initTrans();
  m_renderState.getValuesFromDevice(m_device);
  m_renderState.setDefault();
  m_renderState.setValuesToDevice(m_device);
  m_initDone = true;
}

void D3Scene::close() {
  if(!m_initDone) return;
  PropertyContainer::clear();
  destroyAllLightControls();
  removeAllSceneObjects();
  SAFEDELETE(m_lightsEnabled);
  SAFEDELETE(m_lightsDefined);
  SAFERELEASE(m_device);
  m_initDone = false;
}

void D3Scene::initTrans() {
  m_viewAngel      = radians(45);
  m_nearViewPlane  = 0.1f;
  updateProjMatrix();
  initCameraTrans(D3DXVECTOR3(0,-5,0), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,1));
  initObjTrans();
}

D3Scene &D3Scene::selectMaterial(int materialIndex) {
  if(materialIndex != m_renderState.m_selectedMaterialIndex) {
    if((UINT)materialIndex < m_materials.size()) {
      const MATERIAL &mat = getMaterial(materialIndex);
      FV(m_device->SetMaterial(&mat));
      if (mat.Diffuse.a < 1.0) {
        setCullMode(D3DCULL_CCW)
        .setZEnable(false)
        .setAlphaBlendEnable(true)
        .setSrcBlend(D3DBLEND_SRCALPHA)
        .setDstBlend(D3DBLEND_INVSRCALPHA);
      } else {
        setCullMode(D3DCULL_CCW).setZEnable(true).setAlphaBlendEnable(false);
      }
    }
    m_renderState.m_selectedMaterialIndex = materialIndex;
  }
  return *this;
}

void D3Scene::setCameraPDUS(const D3PosDirUpScale &pdus) {
  setProperty(SP_CAMERAPDUS, m_cameraPDUS, pdus);
}

void D3Scene::setCameraPos(const D3DXVECTOR3 &pos) {
  D3PosDirUpScale newCam = m_cameraPDUS;
  newCam.setPos(pos);
  setCameraPDUS(newCam);
}

void D3Scene::setCameraOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  D3PosDirUpScale newCam = m_cameraPDUS;
  newCam.setOrientation(dir, up);
  setCameraPDUS(newCam);
}

void D3Scene::setCameraLookAt(const D3DXVECTOR3 &point) {
  initCameraTrans(getCameraPos(), point, getCameraUp());
}

void D3Scene::initObjTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up, const D3DXVECTOR3 &scale) {
  m_objectPDUS.setPos(pos)
              .setOrientation(dir, up)
              .setScale(scale);
}

void D3Scene::initCameraTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  D3PosDirUpScale newCam = m_cameraPDUS;
  newCam.setPos(pos)
        .setOrientation(lookAt - pos, up)
        .setScale(D3DXVECTOR3(1,1,1));
  setCameraPDUS(newCam);
}

void D3Scene::updateViewMatrix() {
  setViewMatrix(m_cameraPDUS.getViewMatrix());
}

void D3Scene::setViewAngel(float angel) {
  if(angel > 0 && angel < D3DX_PI) {
    m_viewAngel = angel;
    updateProjMatrix();
  }
}

void D3Scene::setNearViewPlane(float zn) {
  if(zn > 0) {
    m_nearViewPlane = zn;
    updateProjMatrix();
  }
}

String D3Scene::getCameraString() const {
  return format(_T("Camera:View angel:%.1lf, Near view:%.3lf\n%s")
               ,degrees(getViewAngel())
               ,getNearViewPlane()
               ,m_cameraPDUS.toString().cstr()
               );
  }

void D3Scene::updateProjMatrix() {
  const CRect cl = getClientRect(getHwnd());
  D3DXMATRIX matProj;
  setProjMatrix(*D3DXMatrixPerspectiveFov(&matProj, m_viewAngel, (float)cl.Width()/cl.Height(), m_nearViewPlane, 200.0f));
}

void D3Scene::setProjMatrix(const D3DXMATRIX &m) {
  const D3DXMATRIX currentProj = getProjMatrix();
  setTransformation(D3DTS_PROJECTION, m);
  if(m != currentProj) {
    notifyPropertyChanged(SP_PROJECTIONTRANSFORMATION, &currentProj, &m);
  }
}

void D3Scene::setTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m) {
  V(m_device->SetTransform(id, &m));
}

D3DXMATRIX D3Scene::getTransformation(D3DTRANSFORMSTATETYPE id) const {
  D3DXMATRIX m;
  V(m_device->GetTransform(id, &m));
  return m;
}

// -------------------------------- LIGHT ------------------------------------
D3DLIGHT D3Scene::getDefaultLight(D3DLIGHTTYPE type) { // static
  LIGHT result;
  result.setDefault(type);
  return result;
}

BitSet D3Scene::getLightsVisible() const {
  BitSet result(m_maxLightCount);
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    D3SceneObject *obj = m_objectArray[i];
    if((obj->getType() == SOTYPE_LIGHTCONTROL) && obj->isVisible()) {
      result.add(((D3LightControl*)obj)->getLightIndex());
    }
  }
  return result;
}

D3LightControl *D3Scene::setLightControlVisible(UINT index, bool visible) {
  if(!isLightDefined(index)) {
    Message(_T("%s:Light %d is undefined"), __TFUNCTION__, index);
    return NULL;
  }
  D3LightControl *lc = findLightControlByLightIndex(index);
  if(lc == NULL) {
    lc = addLightControl(index);
  }
  if(lc) {
    lc->setVisible(visible);
  }
  return lc;
}

D3LightControl *D3Scene::addLightControl(UINT index) {
  if(!isLightDefined(index)) {
    Message(_T("%s:Light %d is undefined"), __TFUNCTION__, index);
    return NULL;
  }
  D3LightControl *result = findLightControlByLightIndex(index);
  if(result != NULL) return result;
  LIGHT param = getLight(index);
  switch(param.Type) {
  case D3DLIGHT_DIRECTIONAL    : result = new D3LightControlDirectional(*this, index); TRACE_NEW(result); break;
  case D3DLIGHT_POINT          : result = new D3LightControlPoint(      *this, index); TRACE_NEW(result); break;
  case D3DLIGHT_SPOT           : result = new D3LightControlSpot(       *this, index); TRACE_NEW(result); break;
  default                      : throwException(_T("Unknown lighttype for light %d:%d"), index, param.Type);
  }
  addSceneObject(result);
  return result;
}

void D3Scene::destroyLightControl(UINT index) {
  D3LightControl *lc = findLightControlByLightIndex(index);
  if(lc == NULL) return;
  removeSceneObject(lc);
  SAFEDELETE(lc);
}

void D3Scene::destroyAllLightControls() {
  const CompactArray<LIGHT> la = getAllLights();
  for (size_t i = 0; i < la.size(); i++) {
    const LIGHT &l = la[i];
    destroyLightControl(l.m_index);
  }
}

int D3Scene::addLight(const D3DLIGHT &light) {
  const UINT oldCount = getLightCount();
  const UINT index    = getFirstFreeLightIndex();
  V(m_device->SetLight(   index, &light));
  V(m_device->LightEnable(index, TRUE  ));
  m_lightsDefined->add(index);
  m_lightsEnabled->add(index);
  const UINT newCount = oldCount + 1;
  notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
  return index;
}

void D3Scene::removeLight(UINT index) {
  if(!isLightDefined(index)) return;
  if(isLightEnabled(index)) {
    m_lightsEnabled->remove(index);
  }
  destroyLightControl(index);
  const UINT oldCount = getLightCount();
  m_lightsDefined->remove(index);
  V(m_device->LightEnable(index, FALSE));
  const UINT newCount = oldCount - 1;
  notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
}

void D3Scene::setLightEnabled(UINT index, bool enabled) {
  if(!isLightDefined(index)) return;
  LIGHT param = getLight(index);
  param.m_enabled = enabled;
  setLight(param);
}

void D3Scene::setLightDirection(UINT index, const D3DXVECTOR3 &dir) {
  if(!isLightDefined(index)) return;
  LIGHT param = getLight(index);
  param.Direction = unitVector(dir);
  setLight(param);
}

void D3Scene::setLightPosition(UINT index, const D3DXVECTOR3 &pos) {
  if(!isLightDefined(index)) return;
  LIGHT param = getLight(index);
  param.Position = pos;
  setLight(param);
}

void D3Scene::setLight(const LIGHT &param) {
  if(!isLightDefined(param.m_index)) {
    Message(_T("%s:Light %d is undefined")
            ,__TFUNCTION__, param.m_index);
    return;
  }
  const LIGHT oldLp = getLight(param.m_index);
  if(param == oldLp) return;
  V(m_device->SetLight(param.m_index, &param));
  V(m_device->LightEnable(param.m_index, param.m_enabled?TRUE:FALSE));
  if(param.m_enabled != oldLp.m_enabled) {
    if(param.m_enabled) {
      m_lightsEnabled->add(param.m_index);
    } else {
      m_lightsEnabled->remove(param.m_index);
    }
  }
  notifyPropertyChanged(SP_LIGHTPARAMETERS, &oldLp, &param);
}

D3LightControl *D3Scene::findLightControlByLightIndex(int lightIndex) {
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    D3SceneObject *obj = m_objectArray[i];
    if((obj->getType() == SOTYPE_LIGHTCONTROL) && (((D3LightControl*)obj)->getLightIndex() == lightIndex)) {
      return (D3LightControl*)obj;
    }
  }
  return NULL;
}

LIGHT D3Scene::getLight(UINT index) const {
  LIGHT lp;
  if(!isLightDefined(index)) {
    memset(&lp, 0xff, sizeof(lp));
  } else {
    V(m_device->GetLight(index, &lp));
    lp.m_index   = index;
    lp.m_enabled = isLightEnabled(index);
  }
  return lp;
}

const CompactArray<LIGHT> D3Scene::getAllLights() const {
  BitSet lightSet = getLightsDefined();
  CompactArray<LIGHT> result(lightSet.size());
  for (Iterator<size_t> it = lightSet.getIterator(); it.hasNext();) {
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

String D3Scene::getLightString(UINT index) const {
  return isLightDefined(index)
        ? getLight(index).toString()
        : format(_T("Light[%d]:Undefined"), index);
}

String D3Scene::getLightString() const {
  String result;
  BitSet lightSet = getLightsDefined();
  for(Iterator<size_t> it = lightSet.getIterator(); it.hasNext(); ) {
    const UINT index = (UINT)it.next();
    if(result.length()) result += _T("\n");
    result += getLightString(index);
  }
  return result;
}

// ---------------------------- MATERIAL -----------------------------
D3DMATERIAL D3Scene::getDefaultMaterial() { // static
  MATERIAL result;
  result.setDefault();
  return result;
}

int D3Scene::addMaterial(const D3DMATERIAL &material) {
  const int oldCount = getMaterialCount();
  const int index    = getFirstFreeMaterialIndex();
  MATERIAL &m = m_materials[index];
  ((D3DMATERIAL&)m) = material;
  m.m_index = index;
  const int newCount = oldCount+1;
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
  return index;
}

int D3Scene::getFirstFreeMaterialIndex() {
  const size_t n = m_materials.size();
  for (size_t i = 0; i < n; i++) {
    if (m_materials[i].m_index < 0) {
      return (int)i;
    }
  }
  m_materials.add(MATERIAL());
  return (int)n;
}

void D3Scene::removeMaterial(UINT index) {
  if((index == 0) || (index >= m_materials.size())) { // cannot remove material 0
    return;
  }
  const int oldCount = getMaterialCount();
  const int newCount = oldCount-1;
  m_materials[index].m_index = -1;
  unselectMaterial();
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

const BitSet D3Scene::getMaterialsDefined() const {
  const size_t n = m_materials.size();
  BitSet result(n+1);
  for(size_t i = 0; i < n; i++) {
    if(m_materials[i].m_index == i) {
      result.add(i);
    }
  }
  return result;
}

void D3Scene::setMaterial(const MATERIAL &material) {
  if (!material.isDefined()) {
    addMaterial(material);
  } else {
    const UINT index = material.m_index;
    if(index >= m_materials.size()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("index=%u, materialCount=%zd"), index, m_materials.size());
    }
    unselectMaterial();
    setProperty(SP_MATERIALPARAMETERS, m_materials[index], material);
  }
}

String D3Scene::getMaterialString(UINT index) const {
  return m_materials[index].toString();
}

String D3Scene::getMaterialString() const {
  String result;
  BitSet materialSet = getMaterialsDefined();
  for(Iterator<size_t> it = materialSet.getIterator(); it.hasNext(); ) {
    const UINT index = (UINT)it.next();
    if(result.length()) result += _T("\n");
    result += getMaterialString(index);
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

  updateViewMatrix();

  V(m_device->BeginScene());
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    D3SceneObject *obj = m_objectArray[i];
    if(obj->isVisible()) {
      V(m_device->SetTransform(D3DTS_WORLD, &obj->getWorldMatrix()));
      obj->draw();
    }
  }

  V(m_device->EndScene());
  V(m_device->Present(NULL, NULL, NULL, NULL));

  setProperty(SP_RENDERTIME, m_renderTime, Timestamp());
}

D3Ray D3Scene::getPickRay(const CPoint &point) const {
  const CSize winSize = getClientRect(m_hwnd).Size();

  const D3DXMATRIX matProj = getProjMatrix();

  // Compute the vector of the pick ray in screen space
  D3DXVECTOR3 v;
  v.x =  (((2.0f * point.x) / winSize.cx) - 1) / matProj._11 * m_nearViewPlane;
  v.y = -(((2.0f * point.y) / winSize.cy) - 1) / matProj._22 * m_nearViewPlane;
  v.z = -m_nearViewPlane;

  const D3DXMATRIX camWorld = m_cameraPDUS.getWorldMatrix();
  return D3Ray(camWorld*v, v*camWorld);
}

D3SceneObject *D3Scene::getPickedObject(const CPoint &point, long mask, D3DXVECTOR3 *hitPoint, D3PickedInfo *info) const {
  const D3Ray    ray           = getPickRay(point);
  float          minDist       = -1;
  D3SceneObject *closestObject = NULL;
  for(size_t i = 0; i < m_objectArray.size(); i++) {
    D3SceneObject *obj = m_objectArray[i];
    const String name = obj->getName();
    if(!obj->isVisible() || ((mask & PICK_MASK(obj->getType())) == 0)) {
      continue;
    }
    float dist;
    if(obj->intersectsWithRay(ray, dist, info)) {
      if((closestObject == NULL) || (dist < minDist)) {
        closestObject = obj;
        minDist       = dist;
      }
    }
  }
  if(closestObject && hitPoint) {
    *hitPoint = ray.m_orig + minDist * ray.m_dir;
  }
  return closestObject;
}

LPDIRECT3DVERTEXBUFFER D3Scene::allocateVertexBuffer(DWORD fvf, UINT count, UINT *bufferSize) {
  const UINT vertexSize = ::FVFToSize(fvf);
  UINT tmp;
  UINT &totalSize  = bufferSize ? *bufferSize : tmp;
  totalSize = vertexSize*count;
  LPDIRECT3DVERTEXBUFFER result;
  V(m_device->CreateVertexBuffer(totalSize, 0, fvf, D3DPOOL_DEFAULT, &result, NULL));
  TRACE_CREATE(result);
  return result;
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
  } catch (Exception e) {
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
      updateProjMatrix();
    }
  }
}

void D3Scene::addSceneObject(D3SceneObject *obj) {
  m_objectArray.add(obj);
  notifyIfObjectArrayChanged();
}

void D3Scene::removeSceneObject(D3SceneObject *obj) {
  const int index = (int)m_objectArray.getFirstIndex(obj);
  if(index >= 0) {
    if (obj->getType() == SOTYPE_ANIMATEDOBJECT) {
      ((D3AnimatedSurface*)obj)->stopAnimation();
    }
    m_objectArray.remove(index);
    notifyIfObjectArrayChanged();
  }
}

void D3Scene::removeAllSceneObjects() {
  while(getObjectCount() > 0) {
    removeSceneObject(m_objectArray.last());
  }
}

void D3Scene::stopAllAnimations() {
  for (size_t i = 0; i < m_objectArray.size(); i++) {
    D3SceneObject *obj = m_objectArray[i];
    if (obj->getType() == SOTYPE_ANIMATEDOBJECT) {
      ((D3AnimatedSurface*)obj)->stopAnimation();
    }
  }
}

void D3Scene::notifyIfObjectArrayChanged() {
  const int newCount = (int)m_objectArray.size();
  setProperty(SP_OBJECTCOUNT, m_oldObjectCount, newCount);
}

void D3Scene::setAnimationFrameIndex(int &oldValue, int newValue) {
  setProperty(SP_ANIMATIONFRAMEINDEX, oldValue, newValue);
}
