#include "pch.h"
#include <MFCUtil/2DXML.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CPoint      &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, CPoint            &p) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CSize       &s) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("cx"), s.cx);
  doc.setValue(n, _T("cy"), s.cy);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, CSize             &s) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("cx"), s.cx);
  doc.getValue(n, _T("cy"), s.cy);
}
