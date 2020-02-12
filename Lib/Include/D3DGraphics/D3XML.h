#pragma once

#include <XMLDoc.h>
#include <Math/Cube3D.h>
#include "Material.h"
#include "Light.h"
#include "D3Math.h"

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point3DTemplate<T> &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
  doc.setValue(n, _T("z"), p.z);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Point3DTemplate<T> &p) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
  doc.getValue(n, _T("z"), p.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXVECTOR3 &v   );
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       D3DXVECTOR3 &v   );
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DVECTOR   &v   );
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       D3DVECTOR   &v   );

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3PosDirUpScale &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       D3PosDirUpScale &v);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Cube3DTemplate<T> &cube) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("lbn"), cube.LBN());
  setValue(doc, n, _T("rtf"), cube.RTF());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Cube3DTemplate<T> &cube) {
  XMLNodePtr n = doc.getChild(parent, tag);
  Point3DTemplate<T> lbn, rtf;
  getValue(doc, n, _T("lbn"), lbn);
  getValue(doc, n, _T("rtf"), rtf);
  cube = Cube3DTemplate<T>(lbn, rtf);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DCOLORVALUE &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DCOLORVALUE       &v);

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DMATERIAL   &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DMATERIAL         &v);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const MaterialMap   &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, MaterialMap         &v);

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DLIGHT      &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DLIGHT            &v);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const LIGHT         &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, LIGHT               &v);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const LightArray    &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, LightArray          &v);
