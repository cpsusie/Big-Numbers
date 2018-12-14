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

MoveablePointArray GraphZeroesResult::getMoveablePointArray() const {
  const size_t n = m_zeroes.size();
  MoveablePointArray result(n);
  for(size_t i = 0; i < n; i++) {
    result.add(new MoveablePoint(getGraph(), Point2D(m_zeroes[i],0)));
  }
  return result;
}

String GraphZeroesResult::toString(const TCHAR *name) const {
  return format(_T("Zeroes of %s:%s")
                ,name ? name : m_graph.getParam().getDisplayName().cstr()
                ,m_zeroes.toStringBasicType().cstr()
               );
}

MoveablePointArray GraphZeroesResultArray::getMoveablePointArray() const {
  MoveablePointArray result;
  for(size_t i = 0; i < size(); i++) {
    result.addAll((*this)[i].getMoveablePointArray());
  }
  return result;
}

String GraphZeroesResultArray::toString() const {
  if(isEmpty()) {
    return _T("No zeroes found");
  }
  if(size() == 1) {
    return __super::toString(_T("\n"));
  }
  StringArray result;
  for(size_t i = 0; i < size(); i++) {
    const GraphZeroesResult &zr   = (*this)[i];
    const String             name = format(_T("%s.%s")
                                          ,m_graph.getParam().getDisplayName().cstr()
                                          ,zr.getGraph().getParam().getDisplayName().cstr());
    result.add(zr.toString(name.cstr()));
  }
  return result.toString(_T("\n"));
}

MoveablePointArray GraphExtremaResult::getMoveablePointArray() const {
  const size_t n = m_extrema.size();
  MoveablePointArray result(n);
  for(size_t i = 0; i < n; i++) {
    result.add(new MoveablePoint(getGraph(), m_extrema[i]));
  }
  return result;
}

String GraphExtremaResult::toString(const TCHAR *name) const {
  return format(_T("%s of %s:%s")
                ,getExtremaTypeStr()
                ,name ? name : m_graph.getParam().getDisplayName().cstr()
                ,m_extrema.toString().cstr()
               );
}

MoveablePointArray GraphExtremaResultArray::getMoveablePointArray() const {
  MoveablePointArray result;
  for(size_t i = 0; i < size(); i++) {
    result.addAll((*this)[i].getMoveablePointArray());
  }
  return result;
}

String GraphExtremaResultArray::toString() const {
  if(isEmpty()) {
    return format(_T("No %s found"), getExtremaTypeStr());
  }
  if(size() == 1) {
    return __super::toString(_T("\n"));
  }
  StringArray result;
  for(size_t i = 0; i < size(); i++) {
    const GraphExtremaResult &er   = (*this)[i];
    const String             name = format(_T("%s.%s")
                                          ,m_graph.getParam().getDisplayName().cstr()
                                          ,er.getGraph().getParam().getDisplayName().cstr());
    result.add(er.toString(name.cstr()));
  }
  return result.toString(_T("\n"));
}
