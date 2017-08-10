#include "stdafx.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, int rollAvgSize, GraphStyle style) {
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
  String str = toLowerCase(GraphParameters::graphStyleToString(style));
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

const TCHAR *GraphParameters::graphStyleToString(GraphStyle style) {  // static
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

void ExprGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  setValue(doc, root, m_trigonometricMode);
  __super::putDataToDoc(doc);
}

void ExprGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  getValue(doc, root, m_trigonometricMode);
  __super::getDataFromDoc(doc);
}

const TCHAR *ExprGraphParameters::trigonometricModeToString(TrigonometricMode mode) {  // static
  switch(mode) {
  case RADIANS : return _T("Radians");
  case DEGREES : return _T("Degrees");
  case GRADS   : return _T("Grads");
  default      : return _T("Radians");
  }
}

TrigonometricMode ExprGraphParameters::trigonometricModeFromString(const String &str) { // static
  if(str.equalsIgnoreCase(_T("radians"))) {
    return RADIANS;
  } else if(str.equalsIgnoreCase(_T("degrees"))) {
    return DEGREES;
  } else if(str.equalsIgnoreCase(_T("grads"))) {
    return GRADS;
  } else {
    return RADIANS;
  }
}

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode trigoMode) {
  String str = toLowerCase(ExprGraphParameters::trigonometricModeToString(trigoMode));
  doc.setValue(n, _T("trigo"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &trigoMode) {
  String str;
  doc.getValue(n, _T("trigo"), str);
  trigoMode = ExprGraphParameters::trigonometricModeFromString(str);
}
