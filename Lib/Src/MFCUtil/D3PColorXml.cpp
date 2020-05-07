#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/ColorSpace.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3PCOLOR &v) {
  const BYTE r = ARGB_GETRED(  v.m_color);
  const BYTE g = ARGB_GETGREEN(v.m_color);
  const BYTE b = ARGB_GETBLUE( v.m_color);
  const BYTE a = ARGB_GETALPHA(v.m_color);
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("r"      ), r);
  setValue(doc, n, _T("g"      ), g);
  setValue(doc, n, _T("b"      ), b);
  setValue(doc, n, _T("a"      ), a);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3PCOLOR &v) {
  BYTE r, g, b, a;
  XMLNodePtr n = doc.getChild(parent, tag);
  getValue(doc, n, _T("r"      ), r);
  getValue(doc, n, _T("g"      ), g);
  getValue(doc, n, _T("b"      ), b);
  getValue(doc, n, _T("a"      ), a);
  v = D3DCOLOR_ARGB(a, r, g, b);
}
