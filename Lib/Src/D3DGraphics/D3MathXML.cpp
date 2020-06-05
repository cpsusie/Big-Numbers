#include "pch.h"
#include <XMLUtil.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXVECTOR3 &v) {
  setValue(doc, n, _T("x"), v.x);
  setValue(doc, n, _T("y"), v.y);
  setValue(doc, n, _T("z"), v.z);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DXVECTOR3 &v) {
  getValue(doc, n, _T("x"), v.x);
  getValue(doc, n, _T("y"), v.y);
  getValue(doc, n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DVECTOR &v) {
  setValue(doc, n, _T("x"), v.x);
  setValue(doc, n, _T("y"), v.y);
  setValue(doc, n, _T("z"), v.z);
}
void getValue(XMLDoc &doc, XMLNodePtr n, D3DVECTOR &v) {
  getValue(doc, n, _T("x"), v.x);
  getValue(doc, n, _T("y"), v.y);
  getValue(doc, n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXQUATERNION &q) {
  setValue(doc, n, _T("x"), q.x);
  setValue(doc, n, _T("y"), q.y);
  setValue(doc, n, _T("z"), q.z);
  setValue(doc, n, _T("w"), q.w);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DXQUATERNION &q) {
  getValue(doc, n, _T("x"), q.x);
  getValue(doc, n, _T("y"), q.y);
  getValue(doc, n, _T("z"), q.z);
  getValue(doc, n, _T("w"), q.w);
}
