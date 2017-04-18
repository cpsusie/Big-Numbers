#include "stdafx.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style) {
  setName(name);
  m_color    = color;
  m_rollSize = rollSize;
  m_style    = style;
};

void GraphParameters::setStdValues(XMLDoc &doc, XMLNodePtr n) {
  setValue(doc, n, m_style);
  doc.setValue( n, _T("color"      ), format(_T("%08x"), m_color));
  doc.setValue( n, _T("rollsize"   ), m_rollSize   );
}

void GraphParameters::getStdValues(XMLDoc &doc, XMLNodePtr n) {
  getValue(doc, n, m_style);
  String str;
  doc.getValue( n, _T("color"      ), str           );
  _stscanf(str.cstr(), _T("%x"), &m_color);
  doc.getValue( n, _T("rollsize"   ), m_rollSize    );
}

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode trigoMode) {
  String str = toLowerCase(GraphParameters::trigonometricModeToString(trigoMode));
  doc.setValue(n, _T("trigo"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &trigoMode) {
  String str;
  doc.getValue(n, _T("trigo"), str);
  trigoMode = GraphParameters::trigonometricModeFromString(str);
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

const TCHAR *GraphParameters::trigonometricModeToString(TrigonometricMode mode) {  // static
  switch(mode) {
  case RADIANS : return _T("Radians");
  case DEGREES : return _T("Degrees");
  case GRADS   : return _T("Grads");
  default      : return _T("Radians");
  }
}

TrigonometricMode GraphParameters::trigonometricModeFromString(const String &str) { // static
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

