#pragma once

#include <XMLUtil.h>
#include <Math/Rectangle2D.h>
#include <Math/Cube3D.h>

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTemplate<T> &p, Args... args) {
  setValue(doc, n, _T("x"), p.x, args...);
  setValue(doc, n, _T("y"), p.y, args...);
}

template<typename T, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Point2DTemplate<T> &p, Args... args) {
  getValue(doc, n, _T("x"), p.x, args...);
  getValue(doc, n, _T("y"), p.y, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Size2DTemplate<T> &s, Args... args) {
  setValue(doc, n, _T("cx"), s.cx, args...);
  setValue(doc, n, _T("cy"), s.cy, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Size2DTemplate<T> &s, Args... args) {
  getValue(doc, n, _T("cx"), s.cx, args...);
  getValue(doc, n, _T("cy"), s.cy, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Line2DTemplate<T> &l, Args... args) {
  setValue(doc, n, _T("p1"),l.m_p1, args...);
  setValue(doc, n, _T("p2"),l.m_p2, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Line2DTemplate<T> &l, Args... args) {
  getValue(doc, n, _T("p1"), l.m_p1, args...);
  getValue(doc, n, _T("p2"), l.m_p2, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Rectangle2DTemplate<T> &r, Args... args) {
  setValue(doc, n, _T("p"   ), r.m_p   , args...);
  setValue(doc, n, _T("size"), r.m_size, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Rectangle2DTemplate<T> &r, Args... args) {
  getValue(doc, n, _T("p"   ), r.m_p   , args...);
  getValue(doc, n, _T("size"), r.m_size, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Point3DTemplate<T> &p, Args... args) {
  setValue(doc, n, _T("x"), p.x, args...);
  setValue(doc, n, _T("y"), p.y, args...);
  setValue(doc, n, _T("z"), p.z, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Point3DTemplate<T> &p, Args... args) {
  getValue(doc, n, _T("x"), p.x, args...);
  getValue(doc, n, _T("y"), p.y, args...);
  getValue(doc, n, _T("z"), p.z, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Size3DTemplate<T> &s, Args... args) {
  setValue(doc, n, _T("cx"), s.cx, args...);
  setValue(doc, n, _T("cy"), s.cy, args...);
  setValue(doc, n, _T("cz"), s.cz, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Size3DTemplate<T> &s, Args... args) {
  getValue(doc, n, _T("cx"), s.cx, args...);
  getValue(doc, n, _T("cy"), s.cy, args...);
  getValue(doc, n, _T("cz"), s.cz, args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Cube3DTemplate<T> &cube, Args... args) {
  setValue(doc, n, _T("lbn"), cube.LBN(), args...);
  setValue(doc, n, _T("rtf"), cube.RTF(), args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Cube3DTemplate<T> &cube, Args... args) {
  Point3DTemplate<T> lbn, rtf;
  getValue(doc, n, _T("lbn"), lbn, args...);
  getValue(doc, n, _T("rtf"), rtf, args...);
  cube = Cube3DTemplate<T>(lbn, rtf);
}

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode  mode);
void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &mode);
