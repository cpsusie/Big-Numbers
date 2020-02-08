#include "pch.h"
#include <ByteFile.h>
#include <D3DGraphics/D3Scene.h>

void D3Scene::saveState(const String &fileName) const {
  save(ByteOutputFile(fileName));
}

void D3Scene::loadState(const String &fileName) {
  load(ByteInputFile(fileName));
}

void D3Scene::save(ByteOutputStream &s) const {
  s.putBytes(      (BYTE*)(&m_camPDUS       ), sizeof(m_camPDUS        ));
  s.putBytes(      (BYTE*)(&m_defaultObjPDUS), sizeof(m_defaultObjPDUS ));
  s.putBytes(      (BYTE*)(&m_viewAngel     ), sizeof(m_viewAngel      ));
  s.putBytes(      (BYTE*)(&m_nearViewPlane ), sizeof(m_nearViewPlane  ));
  s.putBytes(      (BYTE*)(&m_renderState   ), sizeof(m_renderState));
  m_materialMap.save(s);
  saveLights(s);
}

void D3Scene::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)(&m_camPDUS         ), sizeof(m_camPDUS        ));
  s.getBytesForced((BYTE*)(&m_defaultObjPDUS  ), sizeof(m_defaultObjPDUS ));
  s.getBytesForced((BYTE*)(&m_viewAngel       ), sizeof(m_viewAngel      ));
  s.getBytesForced((BYTE*)(&m_nearViewPlane   ), sizeof(m_nearViewPlane  ));
  s.getBytesForced((BYTE*)(&m_renderState     ), sizeof(m_renderState    ));
  m_materialMap.load(s);
  loadLights(s);
}

void D3Scene::saveLights(ByteOutputStream &s) const {
  getAllLights().save(s);
}

void D3Scene::loadLights(ByteInputStream &s) {
  LightArray lightArray;
  lightArray.load(s);
  removeAllLights();
  for(size_t i = 0; i < lightArray.size(); i++) {
    setLight(lightArray[i]);
  }
}
