#include "pch.h"
#include <PersistentData.h>
#include <D3DGraphics/3DXML.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point3D &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
  doc.setValue(n, _T("z"), p.z);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Point3D &p) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
  doc.getValue(n, _T("z"), p.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXVECTOR3 &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), v.x);
  doc.setValue(n, _T("y"), v.y);
  doc.setValue(n, _T("z"), v.z);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXVECTOR3 &v) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  doc.getValue(n, _T("x"), v.x);
  doc.getValue(n, _T("y"), v.y);
  doc.getValue(n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXCube3 &cube) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("lbn"), cube.getMin());
  setValue(doc, n, _T("rtf"), cube.getMax());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXCube3 &cube) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  D3DXVECTOR3 tmp;
  getValue(doc, n, _T("lbn"), tmp); cube.setMin(tmp);
  getValue(doc, n, _T("rtf"), tmp); cube.setMax(tmp);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Cube3D &cube) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("lbn"), cube.getMin());
  setValue(doc, n, _T("rtf"), cube.getMax());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Cube3D &cube) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  Point3D tmp;
  getValue(doc, n, _T("lbn"), tmp); cube.setMin(tmp);
  getValue(doc, n, _T("rtf"), tmp); cube.setMax(tmp);
}
