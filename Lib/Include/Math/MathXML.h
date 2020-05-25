#pragma once

#include <XMLUtil.h>
#include <Math/Rectangle2D.h>
#include <Math/Cube3D.h>

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTemplate<T> &p) {
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point2DTemplate<T> &p) {
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Size2DTemplate<T> &s) {
  doc.setValue(n, _T("cx"), s.cx);
  doc.setValue(n, _T("cy"), s.cy);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Size2DTemplate<T> &s) {
  doc.getValue(n, _T("cx"), s.cx);
  doc.getValue(n, _T("cy"), s.cy);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Line2DTemplate<T> &l) {
  doc.setValue(n, _T("p1"),l.m_p1);
  doc.setValue(n, _T("p2"),l.m_p2);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Line2DTemplate<T> &l) {
  doc.getValue(n, _T("p1"), l.m_p1);
  doc.getValue(n, _T("p2"), l.m_p2);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Rectangle2DTemplate<T> &r) {
  doc.setValue(n, _T("x"), r.m_x);
  doc.setValue(n, _T("y"), r.m_y);
  doc.setValue(n, _T("w"), r.m_w);
  doc.setValue(n, _T("h"), r.m_h);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Rectangle2DTemplate<T> &r) {
  doc.getValue(n, _T("x"), r.m_x);
  doc.getValue(n, _T("y"), r.m_y);
  doc.getValue(n, _T("w"), r.m_w);
  doc.getValue(n, _T("h"), r.m_h);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point3DTemplate<T> &p) {
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
  doc.setValue(n, _T("z"), p.z);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point3DTemplate<T> &p) {
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
  doc.getValue(n, _T("z"), p.z);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Size3DTemplate<T> &s) {
  doc.setValue(n, _T("cx"), s.cx);
  doc.setValue(n, _T("cy"), s.cy);
  doc.setValue(n, _T("cz"), s.cz);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Size3DTemplate<T> &s) {
  doc.getValue(n, _T("cx"), s.cx);
  doc.getValue(n, _T("cy"), s.cy);
  doc.getValue(n, _T("cz"), s.cz);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Cube3DTemplate<T> &cube) {
  setValue(doc, n, _T("lbn"), cube.LBN());
  setValue(doc, n, _T("rtf"), cube.RTF());
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Cube3DTemplate<T> &cube) {
  Point3DTemplate<T> lbn, rtf;
  getValue(doc, n, _T("lbn"), lbn);
  getValue(doc, n, _T("rtf"), rtf);
  cube = Cube3DTemplate<T>(lbn, rtf);
}

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode  mode);
void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &mode);
