#include "stdafx.h"
#include "D3LightControl.h"

DECLARE_THISFILE;

int D3Scene::m_textureCoordCount;

D3Scene::D3Scene() {
  m_device            = NULL;
  m_oldObjectCount    = 0;
  m_lightsEnabled     = NULL;
  m_lightsDefined     = NULL;
  m_fillMode          = D3DFILL_SOLID;
  m_shadeMode         = D3DSHADE_GOURAUD;
  m_backgroundColor   = D3DCOLOR_COLORVALUE(.8f, .8f, .8f, 1.0f);
  m_initDone          = false;
}

D3Scene::~D3Scene() {
  removeAllSceneObjects();
  if(m_lightsEnabled) {
    delete m_lightsEnabled;
  }
  if(m_lightsDefined) {
    delete m_lightsDefined;
  }
  if(m_device != NULL) {
    m_device->Release();
  }
}

void D3Scene::notifyPropertyChanged(int id, const void *oldValue, const void *newValue) {
  if(m_initDone) {
    PropertyContainer::notifyPropertyChanged(id, oldValue, newValue);
  }
}

void D3Scene::init(HWND hwnd) {
  m_hwnd = hwnd;

  m_device = D3DeviceFactory::createDevice(m_hwnd);

  D3DCAPS9 deviceCaps;
  V(m_device-> GetDeviceCaps(&deviceCaps));

  m_textureCoordCount = deviceCaps.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK;
  m_maxLightCount     = deviceCaps.MaxActiveLights;
  m_lightsEnabled     = new BitSet(m_maxLightCount);
  m_lightsDefined     = new BitSet(m_maxLightCount);

  setLightParam(getDefaultLightParam());
  setMaterial(getDefaultMaterial());

  initTrans();

  enableSpecular(true);
  V(m_device->SetRenderState(D3DRS_ZENABLE         , TRUE));
  V(m_device->SetRenderState(D3DRS_AMBIENT         , D3DCOLOR_XRGB(50, 50, 50)));
  V(m_device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE));
//  V(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE ));

  m_initDone = true;
}

void D3Scene::initTrans() {
  m_viewAngel      = radians(45);
  m_nearViewPlane  = 0.1f;
  updateProjMatrix();
  initCameraTrans(D3DXVECTOR3(0,-5,0), D3DXVECTOR3(0,0,0), D3DXVECTOR3(0,0,1));
  initObjTrans();
}

void D3Scene::setFillMode(D3DFILLMODE fillMode) {
  const D3DFILLMODE oldFillMode = getFillMode();
  m_fillMode = fillMode;
  if(m_fillMode != oldFillMode) {
    notifyPropertyChanged(SP_FILLMODE, &oldFillMode, &m_fillMode);
  }
}

void D3Scene::setShadeMode(D3DSHADEMODE shadeMode) {
  const D3DSHADEMODE oldShadeMode = getShadeMode();
  m_shadeMode = shadeMode;
  if(m_shadeMode != oldShadeMode) {
    notifyPropertyChanged(SP_SHADEMODE, &oldShadeMode, &m_shadeMode);
  }
}

void D3Scene::setObjPos(const D3DXVECTOR3 &pos) {
  const D3DXVECTOR3 oldPos = m_objectPDUS.getPos();
  if(pos != oldPos) {
    m_objectPDUS.setPos(pos);
    notifyPropertyChanged(SP_OBJECTPOS, &oldPos, &pos);
  }
}

void D3Scene::setObjOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  if(dir != m_objectPDUS.getDir() || up != m_objectPDUS.getUp()) {
    const D3PosDirUpScale oldObj = m_objectPDUS;
    m_objectPDUS.setOrientation(dir, up);
    notifyPropertyChanged(SP_OBJECTORIENTATION, &oldObj,  &m_objectPDUS);
  }
}

void D3Scene::setObjScale(const D3DXVECTOR3 &scale) {
  if(scale != getObjScale()) {
    const D3DXVECTOR3 oldScale = getObjScale();
    m_objectPDUS.setScale(scale);
    notifyPropertyChanged(SP_OBJECTSCALE, &oldScale,  &scale);
  }
}

void D3Scene::setCameraPos(const D3DXVECTOR3 &pos) {
  const D3DXVECTOR3 oldPos = m_cameraPDUS.getPos();
  if(pos != oldPos) {
    m_cameraPDUS.setPos(pos);
    notifyPropertyChanged(SP_CAMERAPOS, &oldPos, &pos);
  }
}

void D3Scene::setCameraOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  const D3PosDirUpScale oldCam = m_cameraPDUS;
  m_cameraPDUS.setOrientation(dir, up);
  if(m_cameraPDUS != oldCam) {
    notifyPropertyChanged(SP_CAMERAORIENTATION, &oldCam,  &m_cameraPDUS);
  }
}

void D3Scene::setCameraLookAt(const D3DXVECTOR3 &point) {
  initCameraTrans(getCameraPos(), point, getCameraUp());
}

void D3Scene::initObjTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up, const D3DXVECTOR3 &scale) {
  const D3PosDirUpScale oldObj = m_objectPDUS;
  m_objectPDUS.setPos(pos);
  m_objectPDUS.setOrientation(dir, up);
  m_objectPDUS.setScale(scale);
  if(m_objectPDUS != oldObj) {
    notifyPropertyChanged(SP_OBJECTORIENTATION, &oldObj, &m_objectPDUS);
  }
}

void D3Scene::initCameraTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up) {
  const D3PosDirUpScale oldCam = m_cameraPDUS;
  m_cameraPDUS.setPos(pos);
  m_cameraPDUS.setOrientation(lookAt - pos, up);
  m_cameraPDUS.setScale(D3DXVECTOR3(1,1,1));
  if(m_cameraPDUS != oldCam) {
    notifyPropertyChanged(SP_CAMERAORIENTATION, &oldCam,  &m_cameraPDUS);
  }
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

void D3Scene::updateProjMatrix() {
  const CRect cl = getClientRect(m_hwnd);
  D3DXMATRIX matProj;
  setProjMatrix(*D3DXMatrixPerspectiveFov(&matProj, m_viewAngel, (float)cl.Width()/cl.Height(), m_nearViewPlane, 200.0f));
}

void D3Scene::setProjMatrix(const D3DXMATRIX &m) {
  const D3DXMATRIX currentProj = getProjMatrix();;
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

LIGHT D3Scene::getDefaultLightParam(D3DLIGHTTYPE type) { // static
  LIGHT result;
  switch(type) {
  case D3DLIGHT_DIRECTIONAL    : result = getDefaultDirectionalLight(); break;
  case D3DLIGHT_POINT          : result = getDefaultPointLight();       break;
  case D3DLIGHT_SPOT           : result = getDefaultSpotLight();        break;
  default                      : result = getDefaultDirectionalLight(); break;
  }
  result.m_lightIndex = getFirstFreeLightIndex();
  return result;
}

LIGHT D3Scene::getDefaultDirectionalLight() {  // static
  LIGHT lp;
  ZeroMemory(&lp, sizeof(lp));
  lp.Type         = D3DLIGHT_DIRECTIONAL;
  lp.Ambient      = D3DXCOLOR(   0.11f,  0.10f,  0.19f, 1.0f);
  lp.Diffuse      = D3DXCOLOR(   0.74f,  0.74f,  0.74f, 1.0f);//D3DXCOLOR(   0.17f,  0.13f,  0.78f, 1.0f);
  lp.Specular     = D3DXCOLOR(   0.85f,  0.84f,  0.95f, 1.0f);
  lp.Direction    = unitVector(D3DXVECTOR3(0.175f, -0.385f, -0.88f));
  lp.m_enabled    = true;

  return lp;
}

LIGHT D3Scene::getDefaultPointLight() {  // static
  LIGHT lp;
  ZeroMemory(&lp, sizeof(lp));
  lp.Type         = D3DLIGHT_POINT;
  lp.Diffuse      = D3DXCOLOR(   0.17f,  0.13f,  0.78f, 1.0f);
  lp.Specular     = D3DXCOLOR(   0.85f,  0.84f,  0.95f, 1.0f);
  lp.Position     = D3DXVECTOR3( 1, 1, 1);
  lp.Range        = 100;
  lp.Attenuation0 = 0.16f;
  lp.Attenuation1 = 0.12f;
  lp.m_enabled    = true;
  return lp;
}

LIGHT D3Scene::getDefaultSpotLight() { // static
  LIGHT lp;
  ZeroMemory(&lp, sizeof(lp));
  lp.Type         = D3DLIGHT_SPOT;
  lp.Diffuse      = D3DXCOLOR(   0.5f,  0.5f,  0.5f, 1.0f);
  lp.Position     = D3DXVECTOR3( 1, 1, 1);
  lp.Direction    = -unitVector(lp.Position);
  lp.Range        = 100;
  lp.Falloff      = 1;
  lp.Theta        = radians(10); // inner angle
  lp.Phi          = radians(15); // outer angle
  lp.m_enabled    = true;
  return lp;
}

void LIGHT ::setInnerAngle(float rad) {
  if((rad < 0) || (rad > D3DX_PI)) {
    return;
  }
  Theta = rad;
  if(Theta > Phi) {
    Phi = Theta;
  }
}

void LIGHT ::setOuterAngle(float rad) {
  if((rad < 0) || (rad > D3DX_PI)) {
    return;
  }
  Phi = rad;
  if(Phi < Theta) {
    Theta = Phi;
  }
}

MATERIAL D3Scene::getDefaultMaterial() { // static
  MATERIAL material;
  ZeroMemory(&material, sizeof(MATERIAL));
  material.Ambient  = D3DXCOLOR(0.10f, 0.10f, 0.16f, 1.0f);
  material.Diffuse  = D3DXCOLOR(0.1f, 0.086f, 0.29f, 1.0f);
  material.Specular = D3DXCOLOR(0.835f, 0.808f, 0.95f, 1.0f);
  material.Power    = 9.73f;
  return material;
}

D3DCOLOR D3Scene::getGlobalAmbientColor() const {
  D3DCOLOR color;
  V(m_device->GetRenderState(D3DRS_AMBIENT, &color));
  return color;
}

void D3Scene::setGlobalAmbientColor(D3DCOLOR color) {
  const D3DCOLOR oldColor = getGlobalAmbientColor();
  if(color != oldColor) {
    V(m_device->SetRenderState(D3DRS_AMBIENT, color));
    notifyPropertyChanged(SP_AMBIENTLIGHT, &oldColor, &color);
  }
}

void D3Scene::enableSpecular(bool enabled) {
  const bool oldValue = isSpecularEnabled();
  if(enabled != oldValue) {
    DWORD b = enabled ? TRUE : FALSE;
    V(m_device->SetRenderState( D3DRS_SPECULARENABLE, b));
    notifyPropertyChanged(SP_SPECULARENABLED, &oldValue, &enabled);
  }
}

bool D3Scene::isSpecularEnabled() const {
  DWORD value;
  V(m_device->GetRenderState(D3DRS_SPECULARENABLE, &value));
  return value ? true : false;
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

void D3Scene::setLightControlVisible(unsigned int index, bool visible) {
  D3LightControl *lc = findLightControlByLightIndex(index);
  if(lc == NULL) {
    lc = addLightControl(index);
  }
  if(lc) {
    lc->setVisible(visible);
  }
}

D3LightControl *D3Scene::addLightControl(unsigned int lightIndex) {
  if(!isLightDefined(lightIndex)) {
    return NULL;
  }
  D3LightControl *result = findLightControlByLightIndex(lightIndex);
  if(result != NULL) {
    return result;
  }
  LIGHT param = getLightParam(lightIndex);
  switch(param.Type) {
  case D3DLIGHT_DIRECTIONAL    : result = new D3LightControlDirectional(*this, lightIndex); break;
  case D3DLIGHT_POINT          : result = new D3LightControlPoint(      *this, lightIndex); break;
  case D3DLIGHT_SPOT           : result = new D3LightControlSpot(       *this, lightIndex); break;
  default                      : throwException(_T("Unknown lighttype for light %d:%d"), lightIndex, param.Type);
  }
  addSceneObject(result);
  return result;
}

void D3Scene::destroyLightControl(unsigned int lightIndex) {
  D3LightControl *lc = findLightControlByLightIndex(lightIndex);
  if(lc == NULL) {
    return;
  }
  removeSceneObject(lc);
  delete lc;
}

void D3Scene::removeLight(unsigned int index) {
  if(isLightDefined(index)) {
    if(isLightEnabled(index)) {
      m_lightsEnabled->remove(index);
    }
    destroyLightControl(index);
    const unsigned int oldCount = getLightCount();
    m_lightsDefined->remove(index);
    V(m_device->LightEnable(index, FALSE));
    const unsigned int newCount = oldCount - 1;
    notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
  }
}

void D3Scene::setLightEnabled(unsigned int index, bool enabled) {
  if(!isLightDefined(index)) {
    return;
  }
  LIGHT param = getLightParam(index);
  param.m_enabled = enabled;
  setLightParam(param);
}

void D3Scene::setLightDirection(unsigned int index, const D3DXVECTOR3 &dir) {
  if(!isLightDefined(index)) {
    return;
  }
  LIGHT param = getLightParam(index);
  param.Direction = unitVector(dir);
  setLightParam(param);
}

void D3Scene::setLightPosition( unsigned int index, const D3DXVECTOR3 &pos) {
  if(!isLightDefined(index)) {
    return;
  }
  LIGHT param = getLightParam(index);
  param.Position = pos;
  setLightParam(param);
}

void D3Scene::setLightParam(const LIGHT &param) {
  if((param.m_lightIndex < 0) || (param.m_lightIndex >= m_maxLightCount)) {
    return;
  }
  if(!isLightDefined(param.m_lightIndex)) {
    const unsigned int oldCount = getLightCount();
    V(m_device->SetLight(param.m_lightIndex, &param));
    V(m_device->LightEnable(param.m_lightIndex, param.m_enabled?TRUE:FALSE));
    m_lightsDefined->add(param.m_lightIndex);
    if(param.m_enabled) {
      m_lightsEnabled->add(param.m_lightIndex);
    }
    const unsigned int newCount = oldCount + 1;
    notifyPropertyChanged(SP_LIGHTCOUNT, &oldCount, &newCount);
  } else {
    const LIGHT oldLp = getLightParam(param.m_lightIndex);
    if(param != oldLp) {
      V(m_device->SetLight(param.m_lightIndex, &param));
      V(m_device->LightEnable(param.m_lightIndex, param.m_enabled?TRUE:FALSE));
      if(param.m_enabled != oldLp.m_enabled) {
        if(param.m_enabled) m_lightsEnabled->add(param.m_lightIndex); else m_lightsEnabled->remove(param.m_lightIndex);
      }
      notifyPropertyChanged(SP_LIGHTPARAMETERS, &oldLp, &param);
    }
  }
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

LIGHT D3Scene::getLightParam(unsigned int index) const {
  LIGHT lp;
  if(!isLightDefined(index)) {
    memset(&lp, 0xff, sizeof(lp));
  } else {
    V(m_device->GetLight(index, &lp));
  }
  lp.m_lightIndex = index;
  lp.m_enabled    = isLightEnabled(index);
  return lp;
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

String D3Scene::getLightString(unsigned int index) const {
  if(!isLightDefined(index)) {
    return "undefined";
  } else {
    return toString(getLightParam(index)) + (isLightEnabled(index) ? ":on":":off");
  }
}

String D3Scene::getLightString() const {
  String result;
  BitSet lightSet = getLightsDefined();
  for(Iterator<unsigned int> it = lightSet.getIterator(); it.hasNext(); ) {
    unsigned int index = it.next();
    if(result.length()) result += "\n";
    result += format(_T("Light %2d:%s"), index, getLightString(index).cstr());
  }
  return result;
}

String toString(const LIGHT &light) {
  String colStr = format(_T("Amb:%s, Dif:%s, Spec:%s")
                        ,toString(light.Ambient).cstr()
                        ,toString(light.Diffuse).cstr()
                        ,toString(light.Specular).cstr()
                        );
  switch(light.Type) {
  case D3DLIGHT_DIRECTIONAL: return format(_T("Dir  :%s Colors:%s"), toString(light.Direction).cstr(), colStr.cstr());
  case D3DLIGHT_POINT      : return format(_T("Point:%s, Range:%.2f Colors:%s"), toString(light.Position).cstr(), light.Range, colStr.cstr());
  case D3DLIGHT_SPOT       : return format(_T("Spot. Pos:%s, Dir:%s, Range:%.2f Inner:%.1f, Outer:%.1f Colors:%s")
                                          ,toString(light.Position).cstr()
                                          ,toString(light.Direction).cstr()
                                          ,light.Range
                                          ,D3DXToDegree(light.Theta)
                                          ,D3DXToDegree(light.Phi)
                                          ,colStr.cstr()
                                          );
  }
  return "";
}

String toString(const MATERIAL  &material) {
  return format(_T("Mat:Amb:%s, Dif:%s, Spec:%s Emi:%s, Pow:%.2f")
               ,toString(material.Ambient).cstr()
               ,toString(material.Diffuse).cstr()
               ,toString(material.Specular).cstr()
               ,toString(material.Emissive).cstr()
               ,material.Power
               );
}

String toString(D3PCOLOR c) {
  const D3DCOLOR cc = c;
  return format(_T("R:%3d G:%dd B:%3d"), RGB_GETRED(cc), RGB_GETGREEN(cc), RGB_GETBLUE(cc));
}

String toString(const D3DCOLORVALUE &c) {
  return format(_T("R:%.2f G:%.2f B:%.2f"), c.r,c.g,c.b);
}

void D3Scene::setMaterial(const MATERIAL &material) {
  const MATERIAL oldMaterial = m_material;
  m_material = material;
  if(m_material != oldMaterial) {
    notifyPropertyChanged(SP_MATERIALPARAMETERS, &oldMaterial, &m_material);
  }
}

String D3Scene::getMaterialString() const {
  return toString(m_material);
}

void D3Scene::setBackgroundColor(D3DCOLOR color) {
  const D3DCOLOR oldBackgroundColor = m_backgroundColor;
  m_backgroundColor = color;
  if(m_backgroundColor != oldBackgroundColor) {
    notifyPropertyChanged(SP_BACKGROUNDCOLOR, &oldBackgroundColor, &m_backgroundColor);
  }
}

void D3Scene::render() {
  V(m_device->Clear(0
                   ,NULL
                   ,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
                   ,m_backgroundColor
                   ,1.0f
                   ,0
                   ));

  updateViewMatrix();
  V(m_device->SetRenderState(D3DRS_LIGHTING        , TRUE));

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

  const Timestamp oldRenderTime = m_renderTime;
  m_renderTime = Timestamp();
  notifyPropertyChanged(SP_RENDERTIME, &oldRenderTime, &m_renderTime);
}

D3Ray D3Scene::getPickRay(const CPoint &point) {
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

D3SceneObject *D3Scene::getPickedObject(const CPoint &point, long mask, D3DXVECTOR3 *hitPoint, D3PickedInfo *info) {
  const D3Ray ray = getPickRay(point);
  float        minDist       = -1;
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

void D3Scene::OnSize() {
  if(m_device) {
    D3DPRESENT_PARAMETERS present = D3DeviceFactory::getDefaultPresentParameters(m_hwnd);
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
  int index = m_objectArray.getFirstIndex(obj);
  if(index >= 0) {
    m_objectArray.remove(index);
  }
  notifyIfObjectArrayChanged();
}

void D3Scene::removeAllSceneObjects() {
  m_objectArray.clear();
  notifyIfObjectArrayChanged();
}

void D3Scene::notifyIfObjectArrayChanged() {
  const int newCount = m_objectArray.size();
  if(newCount != m_oldObjectCount) {
    notifyPropertyChanged(SP_OBJECTCOUNT, &m_oldObjectCount, &newCount);
    m_oldObjectCount = newCount;
  }
}

void D3Scene::setAnimationFrameIndex(int oldValue, int newValue) {
  if(newValue != oldValue) {
    notifyPropertyChanged(SP_ANIMATIONFRAMEINDEX, &oldValue, &newValue);
  }
}
