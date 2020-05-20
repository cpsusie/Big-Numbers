#include "pch.h"
#include <MFCUtil/PixRect.h>
#include <MFCUtil/2DXML.h>

typedef struct {
  const short  m_type;
  const TCHAR *m_name;
} CurveType;

static const CurveType typeNames[] = {
  TT_PRIM_LINE   , _T("line")
 ,TT_PRIM_QSPLINE, _T("qspline")
 ,TT_PRIM_CSPLINE, _T("cspline")
};

static const TCHAR *typeToName(short type) {
  for(CurveType ct : typeNames) {
    if(ct.m_type == type) {
      return ct.m_name;
    }
  }
  return EMPTYSTRING;
}

static short nameToType(const TCHAR *name) {
  for(CurveType ct : typeNames) {
    if(_tcsicmp(ct.m_name,name) == 0) {
      return ct.m_type;
    }
  }
  return 0;
}

void setValue(XMLDoc &doc, XMLNodePtr n, const PolygonCurve &v) {
  doc.setValue(n, _T("type"), typeToName(v.getType()));
  XMLNodePtr plist = doc.createNode(n, _T("points"));
  setValue<Point2DArray,Point2D>(doc, plist, v.getAllPoints());
}

void getValue(XMLDoc &doc, XMLNodePtr n, PolygonCurve &v) {
  String typeName;
  doc.getValue(n, _T("type"), typeName);
  v.m_type = nameToType(typeName.cstr());
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
