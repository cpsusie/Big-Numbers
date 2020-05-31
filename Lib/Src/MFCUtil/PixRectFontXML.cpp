#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/MFCXML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const PolygonCurve &v) {
  setValue(doc, n, _T("type"), _PolygonCurveTypeName::typeToStr(v.getType()));
  XMLNodePtr plist = doc.createNode(n, _T("points"));
  setValue<Point2DArray,Point2D>(doc, plist, v.getAllPoints());
}

void getValue(XMLDoc &doc, XMLNodePtr n, PolygonCurve &v) {
  String typeStr;
  getValue(doc, n, _T("type"), typeStr);
  v.m_type = _PolygonCurveTypeName::strToType(typeStr.cstr());
  XMLNodePtr plist = doc.getChild(n, _T("points"));
  getValue<Point2DArray,Point2D>(doc, plist, v.m_points);

}

void setValue(XMLDoc &doc, XMLNodePtr n, const GlyphPolygon &v) {
  setValue(doc, n, _T("start"), v.m_start);
  XMLNodePtr clist = doc.createNode(n, _T("polygoncurve"));
  setValue<Array<PolygonCurve>, PolygonCurve>(doc, clist, v.getCurveArray());
}

void getValue(XMLDoc &doc, XMLNodePtr n, GlyphPolygon &v) {
  getValue(doc, n, _T("start"), v.m_start);
  XMLNodePtr clist = doc.getChild(n, _T("polygoncurve"));
  getValue<Array<PolygonCurve>, PolygonCurve>(doc, clist, v.m_polygonCurveArray);

}

void setValue(XMLDoc &doc, XMLNodePtr parent, const GlyphCurveData &v) {
  XMLNodePtr n = doc.createNode(parent, _T("glyphcurvedata"));
  setValue<Array<GlyphPolygon>, GlyphPolygon>(doc, n, v.getPolygonArray());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, GlyphCurveData &v) {
  XMLNodePtr n = doc.getChild(parent, _T("glyphcurvedata"));
  getValue<Array<GlyphPolygon>, GlyphPolygon>(doc, n, v.m_glyphPolygonArray);
}
