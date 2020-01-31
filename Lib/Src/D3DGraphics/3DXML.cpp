#include "pch.h"
#include <D3DGraphics/3DXML.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXVECTOR3 &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), v.x);
  doc.setValue(n, _T("y"), v.y);
  doc.setValue(n, _T("z"), v.z);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXVECTOR3 &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), v.x);
  doc.getValue(n, _T("y"), v.y);
  doc.getValue(n, _T("z"), v.z);
}
