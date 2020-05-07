#pragma once

#include <XMLDoc.h>
#include <Math/Rectangle2D.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CPoint      &p);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       CPoint      &p);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const CSize       &s);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       CSize       &s);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point2DTemplate<T> &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Point2DTemplate<T> &p) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Rectangle2DTemplate<T> &r) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), r.m_x);
  doc.setValue(n, _T("y"), r.m_y);
  doc.setValue(n, _T("w"), r.m_w);
  doc.setValue(n, _T("h"), r.m_h);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Rectangle2DTemplate<T> &r) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), r.m_x);
  doc.getValue(n, _T("y"), r.m_y);
  doc.getValue(n, _T("w"), r.m_w);
  doc.getValue(n, _T("h"), r.m_h);
}

class D3PCOLOR;

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3PCOLOR      &v);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3PCOLOR            &v);
