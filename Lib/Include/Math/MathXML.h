#pragma once

#include <XMLUtil.h>
#include "Rectangle2D.h"
#include "Cube3D.h"
#include "CubeN.h"

template<typename T, UINT dimension, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const SizeNTemplate<T, dimension> &s, Args... args) {
  setValue(doc, n, _T("dim"), dimension, args...);
  for(UINT i = 0; i < dimension; i++) {
    setValue(doc, n, format(_T("s%u"), i).cstr(), s[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, SizeNTemplate<T, dimension> &s, Args... args) {
  UINT dim;
  getValue(doc, n, _T("dim"), dim, args...);
  if(dim != dimension) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Dimension mismatch. s.dimension=%u, read dimension=%u"), dimension, dim);
  }
  for(UINT i = 0; i < dimension; i++) {
    getValue(doc, n, format(_T("s%u"), i).cstr(), s[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const PointNTemplate<T, dimension> &p, Args... args) {
  setValue(doc, n, _T("dim"), dimension, args...);
  for(UINT i = 0; i < dimension; i++) {
    setValue(doc, n, format(_T("x%u"), i).cstr(), p[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, PointNTemplate<T, dimension> &p, Args... args) {
  UINT dim;
  getValue(doc, n, _T("dim"), dim, args...);
  if(dim != dimension) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Dimension mismatch. p.dimension=%u, read dimension=%u"), dimension, dim);
  }
  for(UINT i = 0; i < dimension; i++) {
    getValue(doc, n, format(_T("x%u"), i).cstr(), p[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const CubeNTemplate<T, dimension> &c, Args... args) {
  setValue(doc, n, _T("p0"  ), c.p0()  , args...);
  setValue(doc, n, _T("size"), c.size(), args...);
}

template<typename T, UINT dimension, typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, CubeNTemplate<T, dimension> &c, Args... args) {
  getValue(doc, n, _T("p0"  ), c.p0()  , args...);
  getValue(doc, n, _T("size"), c.size(), args...);
}




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
