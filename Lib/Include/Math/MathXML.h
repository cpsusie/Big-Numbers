#pragma once

#include <XMLUtil.h>
#include <Math/Rectangle2D.h>
#include <Math/Cube3D.h>

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTemplate<T> &p) {
  setValue(doc, n, _T("x"), p.x);
  setValue(doc, n, _T("y"), p.y);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point2DTemplate<T> &p) {
  getValue(doc, n, _T("x"), p.x);
  getValue(doc, n, _T("y"), p.y);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Size2DTemplate<T> &s) {
  setValue(doc, n, _T("cx"), s.cx);
  setValue(doc, n, _T("cy"), s.cy);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Size2DTemplate<T> &s) {
  getValue(doc, n, _T("cx"), s.cx);
  getValue(doc, n, _T("cy"), s.cy);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Line2DTemplate<T> &l) {
  setValue(doc, n, _T("p1"),l.m_p1);
  setValue(doc, n, _T("p2"),l.m_p2);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Line2DTemplate<T> &l) {
  getValue(doc, n, _T("p1"), l.m_p1);
  getValue(doc, n, _T("p2"), l.m_p2);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Rectangle2DTemplate<T> &r) {
  setValue(doc, n, _T("x"), r.m_x);
  setValue(doc, n, _T("y"), r.m_y);
  setValue(doc, n, _T("w"), r.m_w);
  setValue(doc, n, _T("h"), r.m_h);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Rectangle2DTemplate<T> &r) {
  getValue(doc, n, _T("x"), r.m_x);
  getValue(doc, n, _T("y"), r.m_y);
  getValue(doc, n, _T("w"), r.m_w);
  getValue(doc, n, _T("h"), r.m_h);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point3DTemplate<T> &p) {
  setValue(doc, n, _T("x"), p.x);
  setValue(doc, n, _T("y"), p.y);
  setValue(doc, n, _T("z"), p.z);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point3DTemplate<T> &p) {
  getValue(doc, n, _T("x"), p.x);
  getValue(doc, n, _T("y"), p.y);
  getValue(doc, n, _T("z"), p.z);
}

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Size3DTemplate<T> &s) {
  setValue(doc, n, _T("cx"), s.cx);
  setValue(doc, n, _T("cy"), s.cy);
  setValue(doc, n, _T("cz"), s.cz);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Size3DTemplate<T> &s) {
  getValue(doc, n, _T("cx"), s.cx);
  getValue(doc, n, _T("cy"), s.cy);
  getValue(doc, n, _T("cz"), s.cz);
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
