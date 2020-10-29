#pragma once

#include <XMLUtil.h>
#include "CubeTemplate.h"
#include "Point2D.h"

template<typename T, UINT dimension, typename... Args>
void setValue(XMLDoc &doc, XMLNodePtr n, const SizeTemplate<T, dimension> &s, Args... args)
{ TCHAR name[3] = { 'c', 'x', 0 }, &ch = name[1];
  for(UINT i = 0; i < dimension; i++, ch++) {
    setValue(doc, n, name, s[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args>
void getValue(XMLDoc &doc, XMLNodePtr n, SizeTemplate<T, dimension> &s, Args... args)
{ TCHAR name[3] = { 'c', 'x', 0 }, &ch = name[1];
  for(UINT i = 0; i < dimension; i++, ch++) {
    getValue(doc, n, name, s[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args>
void setValue(XMLDoc &doc, XMLNodePtr n, const PointTemplate<T, dimension> &p, Args... args)
{ TCHAR name[2] = { 'x', 0 }, &ch = name[0];
  for(UINT i = 0; i < dimension; i++, ch++) {
    setValue(doc, n, name, p[i], args...);
  }
}

template<typename T, UINT dimension, typename... Args>
void getValue(XMLDoc &doc, XMLNodePtr n, PointTemplate<T, dimension> &p, Args... args)
{ TCHAR name[2] = { 'x', 0 }, &ch = name[0];
  for(UINT i = 0; i < dimension; i++, ch++) {
    getValue(doc, n, name, p[i], args...);
  }
}

template<typename PT, typename ST, typename T, UINT dimension, typename... Args>
void setValue(XMLDoc &doc, XMLNodePtr n, const CubeTemplate<PT, ST, T, dimension> &c, Args... args)
{ setValue(doc, n, _T("p0"  ), c.p0()  , args...);
  setValue(doc, n, _T("size"), c.size(), args...);
}

template<typename PT, typename ST, typename T, UINT dimension, typename... Args>
void getValue(XMLDoc &doc, XMLNodePtr n, CubeTemplate<PT, ST, T, dimension> &c, Args... args)
{ getValue(doc, n, _T("p0"  ), c.p0()  , args...);
  getValue(doc, n, _T("size"), c.size(), args...);
}

template<typename T,typename... Args> void setValue(XMLDoc &doc, XMLNodePtr n, const Line2DTemplate<T> &l, Args... args) {
  setValue(doc, n, _T("p1"),l.m_p1, args...);
  setValue(doc, n, _T("p2"),l.m_p2, args...);
}

template<typename T,typename... Args> void getValue(XMLDoc &doc, XMLNodePtr n, Line2DTemplate<T> &l, Args... args) {
  getValue(doc, n, _T("p1"), l.m_p1, args...);
  getValue(doc, n, _T("p2"), l.m_p2, args...);
}

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode  mode);
void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &mode);
