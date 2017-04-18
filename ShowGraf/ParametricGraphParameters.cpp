#include "stdafx.h"
#include <XMLUtil.h>
#include "ParametricGraph.h"

ParametricGraphParameters::ParametricGraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style, TrigonometricMode trigonomtetricMode) 
: GraphParameters(name,color,rollSize,style) {
  m_trigonometricMode = trigonomtetricMode;
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void ParametricGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("ParametricCurve"));
  doc.setValue( root, _T("exprx"     ), m_exprX            );
  doc.setValue( root, _T("expry"     ), m_exprY            );
  setValue(doc, root, _T("interval"  ), m_interval         );
  doc.setValue( root, _T("steps"     ), m_steps            );
  setValue(doc, root,                   m_trigonometricMode);
  setStdValues(doc, root);
}

void ParametricGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("ParametricCurve"));
  doc.getValue( root, _T("exprx"     ), m_exprX            );
  doc.getValue( root, _T("expry"     ), m_exprY            );
  getValue(doc, root, _T("interval"  ), m_interval         );
  doc.getValue( root, _T("steps"     ), m_steps            );
  getValue(doc, root                  , m_trigonometricMode);
  getStdValues(doc, root);
}
