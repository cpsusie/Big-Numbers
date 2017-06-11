#include "pch.h"
#include <PersistentData.h>
#include <MFCUtil/2DXML.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point2D &p) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Point2D &p) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Rectangle2D &r) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), r.m_x);
  doc.setValue(n, _T("y"), r.m_y);
  doc.setValue(n, _T("w"), r.m_w);
  doc.setValue(n, _T("h"), r.m_h);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Rectangle2D &r) {
  XMLNodePtr n = PersistentData::getChild(doc, parent, tag);
  doc.getValue(n, _T("x"), r.m_x);
  doc.getValue(n, _T("y"), r.m_y);
  doc.getValue(n, _T("w"), r.m_w);
  doc.getValue(n, _T("h"), r.m_h);
}
