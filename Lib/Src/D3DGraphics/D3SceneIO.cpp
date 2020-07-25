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
  setValue(doc, root, _T("setup"), *this);
}

void D3Scene::load(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  getValue(doc, root, _T("setup"), *this);
}
