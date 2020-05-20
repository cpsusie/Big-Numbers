#pragma once

#include <XMLUtil.h>
#include <Math/Rectangle2D.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const CPoint      &p);
void getValue(XMLDoc &doc, XMLNodePtr n,       CPoint      &p);
void setValue(XMLDoc &doc, XMLNodePtr n, const CSize       &s);
void getValue(XMLDoc &doc, XMLNodePtr n,       CSize       &s);

template<typename T> void setValue(XMLDoc &doc, XMLNodePtr n, const Point2DTemplate<T> &p) {
  doc.setValue(n, _T("x"), p.x);
  doc.setValue(n, _T("y"), p.y);
}

template<typename T> void getValue(XMLDoc &doc, XMLNodePtr n, Point2DTemplate<T> &p) {
  doc.getValue(n, _T("x"), p.x);
  doc.getValue(n, _T("y"), p.y);
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

class D3PCOLOR;

void setValue(XMLDoc &doc, XMLNodePtr n, const D3PCOLOR       &v);
void getValue(XMLDoc &doc, XMLNodePtr n, D3PCOLOR             &v);

class PolygonCurve;
class GlyphPolygon;
class GlyphCurveData;

void setValue(XMLDoc &doc, XMLNodePtr n, const PolygonCurve   &v);
void getValue(XMLDoc &doc, XMLNodePtr n, PolygonCurve         &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphPolygon   &v);
void getValue(XMLDoc &doc, XMLNodePtr n, GlyphPolygon         &v);

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphCurveData &v);
void getValue(XMLDoc &doc, XMLNodePtr n, GlyphCurveData       &v);
