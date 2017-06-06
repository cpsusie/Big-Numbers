#include "pch.h"
#include <PersistentParameter.h>
#include <D3DGraphics/3DXML.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point3D &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
  doc.setValue(n, _T("z"), p.z);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Point3D &p) {
  XMLNodePtr n = PersistentParameter::getChild(doc, parent, tag);
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
  XMLNodePtr n = PersistentParameter::getChild(doc, parent, tag);
  doc.getValue(n, _T("x"), v.x);
  doc.getValue(n, _T("y"), v.y);
  doc.getValue(n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXCube3 &cube) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("lbn"), cube.m_lbn);
  setValue(doc, n, _T("rtf"), cube.m_rtf);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXCube3 &cube) {
  XMLNodePtr n = PersistentParameter::getChild(doc, parent, tag);
  getValue(doc, n, _T("lbn"), cube.m_lbn);
  getValue(doc, n, _T("rtf"), cube.m_rtf);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Cube3D &cube) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("lbn"), cube.m_lbn);
  setValue(doc, n, _T("rtf"), cube.m_rtf);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Cube3D &cube) {
  XMLNodePtr n = PersistentParameter::getChild(doc, parent, tag);
  getValue(doc, n, _T("lbn"), cube.m_lbn);
  getValue(doc, n, _T("rtf"), cube.m_rtf);
}
