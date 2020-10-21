#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3Scene.h>

// -------------------------------- D3Light ------------------------------------

D3LightControl *D3Scene::addLightControl(UINT lightIndex) {
  if(!isLightDefined(lightIndex)) {
    showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__, lightIndex);
    return nullptr;
  }
  D3LightControl *result = findLightControlByLightIndex(lightIndex);
  if(result != nullptr) return result;
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
  if(lc == nullptr) return;
  removeVisual(lc);
  SAFEDELETE(lc);
}

void D3Scene::destroyAllLightControls() {
  LightArray la = getAllLights();
  for(D3Light l : la) {
    destroyLightControl(l.getIndex());
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
  getDevice().enableLight(lightIndex, false);
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

void D3Scene::setLight(const D3Light &param, bool force) {
  const int index = param.getIndex();
  D3Light oldLp;
  if(force) {
    m_lightsDefined.add(index);
    m_lightsEnabled.setIsMember(index,param.isEnabled());
    getDevice().setLight(param);
    oldLp.setUndefined();
  } else {
    if(!isLightDefined(index)) {
      showWarning(_T("%s:Light %d is undefined"),__TFUNCTION__, index);
      return;
    }
    oldLp = getLight(index);
    if(param == oldLp) return;
    getDevice().setLight(param);
    m_lightsEnabled.setIsMember(index,param.isEnabled());
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
  return nullptr;
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

void D3Scene::setAllLights(const LightArray &a) {
  const bool notifyEnable = setNotifyEnable(false);
  removeAllLights();
  for(D3Light l : a) {
    setLight(l, true);
  }
  setNotifyEnable(notifyEnable);
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
  const LightArray allLights = getAllLights();
  for(const D3Light l : allLights) {
    if(result.length()) result += _T("\n");
    result += l.toString();
  }
  return result;
}
