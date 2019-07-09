#include "stdafx.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style) {
  setName(name);
  m_color       = color;
  m_rollAvgSize = rollAvgSize;
  m_style       = style;
};

void GraphParameters::putDataToDoc(  XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  setValue(doc, root, m_style);
  doc.setValue( root, _T("color"      ), format(_T("%08x"), m_color));
  doc.setValue( root, _T("rollsize"   ), m_rollAvgSize   );
}

void GraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  getValue(doc, root, m_style);
  String str;
  doc.getValue( root, _T("color"      ), str           );
  _stscanf(str.cstr(), _T("%x"), &m_color);
  doc.getValue( root, _T("rollsize"   ), m_rollAvgSize );
}

void setValue(XMLDoc &doc, XMLNodePtr n, GraphStyle style) {
  String str = toLowerCase(GraphParameters::graphStyleToStr(style));
  doc.setValue(n, _T("style"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, GraphStyle &style) {
  String str;
  doc.getValue(n, _T("style"), str);
  style = GraphParameters::graphStyleFromString(str);
}

static const TCHAR *styleName[] = {
  _T("Curve")
 ,_T("Point")
 ,_T("Cross")
};

const TCHAR *GraphParameters::graphStyleToStr(GraphStyle style) {  // static
  if(style < 0 || style > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("style=%d. Must be [0..2]"), style);
  }
  return styleName[style];
}

GraphStyle GraphParameters::graphStyleFromString(const String &s) {  // static
  for(int i = 0; i < ARRAYSIZE(styleName); i++) {
    if(s.equalsIgnoreCase(styleName[i])) {
      return (GraphStyle)i;
    }
  }
  return GSCURVE;
}
