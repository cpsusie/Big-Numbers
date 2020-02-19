#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/D3Scene.h>

void D3Scene::save(const String &fileName) const {
  XMLDoc doc;
  save(doc);
  doc.saveToFile(fileName);
}

void D3Scene::load(const String &fileName) {
  XMLDoc doc;
  doc.loadFromFile(fileName);
  load(doc);
}


void D3Scene::save(XMLDoc &doc) const {
  XMLNodePtr root = doc.createRoot(_T("scene"));
//  setValue(doc, root, _T("camera"       ), m_camPDUS       );
//  setValue(doc, root, _T("defaultobj"   ), m_defaultObjPDUS);
//  doc.setValue( root, _T("viewangel"    ), m_viewAngel     );
//  doc.setValue( root, _T("nearviewplane"), m_nearViewPlane );
  saveMaterials(doc, root);
  saveLights(   doc, root);
}

void D3Scene::load(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
//  getValue(doc, root, _T("camera"       ), m_camPDUS       );
//  getValue(doc, root, _T("defaultobj"   ), m_defaultObjPDUS);
//  doc.getValue( root, _T("viewangel"    ), m_viewAngel     );
//  doc.getValue( root, _T("nearviewplane"), m_nearViewPlane );
  loadMaterials(doc, root);
  loadLights(   doc, root);
}

void D3Scene::saveLights(XMLDoc &doc, XMLNodePtr parent) const {
  setValue(doc, parent, _T("lights"), getAllLights());
}

void D3Scene::loadLights(XMLDoc &doc, XMLNodePtr parent) {
  LightArray a;
  getValue(doc, parent, _T("lights"), a);
  removeAllLights();
  for(size_t i = 0; i < a.size(); i++) {
    const LIGHT &light = a[i];
    setLightEnabled(addLight(light), light.isEnabled());
  }
}

void D3Scene::saveMaterials(XMLDoc &doc, XMLNodePtr parent) const {
  setValue(doc, parent, _T("materials"), m_materialMap);
}

void D3Scene::loadMaterials(XMLDoc &doc, XMLNodePtr parent) {
  MaterialMap map;
  getValue(doc, parent, _T("materials"), map);
  m_materialMap = map;
}
