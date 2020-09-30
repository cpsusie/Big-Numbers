#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/MFCXML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const PolygonCurve2D &v) {
  setValue(doc, n, _T("type"  ), _PolygonCurveTypeName::typeToStr(v.getType()));
  setValue(doc, n, _T("points"), ((Point2DArray&)v.m_points).getIterator());
}

void getValue(XMLDoc &doc, XMLNodePtr n, PolygonCurve2D &v) {
  String typeStr;
  getValue(doc, n, _T("type"  ), typeStr);
  v.m_type = _PolygonCurveTypeName::strToType(typeStr.cstr());
  getValue(doc, n, _T("points"), v.m_points);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphPolygon &v) {
  setValue(doc, n, _T("start"       ), v.getStart());
  setValue(doc, n, _T("polygoncurve"), v.getCurveArray());
}

void getValue(XMLDoc &doc, XMLNodePtr n, GlyphPolygon &v) {
  Point2D               start;
  Array<PolygonCurve2D> curveArray;
  getValue(doc, n, _T("start"), start);
  getValue(doc, n, _T("polygoncurve"), curveArray);
  v = GlyphPolygon(start, curveArray);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const GlyphCurveData &v) {
  setValue(doc, parent, _T("glyphcurvedata"), v.getPolygonArray());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, GlyphCurveData &v) {
  Array<GlyphPolygon> polygonArray;
  getValue(doc, parent, _T("glyphcurvedata"), polygonArray);
  v = GlyphCurveData(polygonArray);
}
