#include "stdafx.h"
#include <XMLUtil.h>
#include "ParametricGraph.h"

ParametricGraphParameters::ParametricGraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style, TrigonometricMode trigonometricMode)
: ExprGraphParameters(name,color,rollAvgSize,style, trigonometricMode) {
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void ParametricGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("ParametricCurve"));
  doc.setValue( root, _T("common"    ), m_commonText       );
  doc.setValue( root, _T("exprx"     ), m_exprX            );
  doc.setValue( root, _T("expry"     ), m_exprY            );
  setValue(doc, root, _T("interval"  ), m_interval         );
  doc.setValue( root, _T("steps"     ), m_steps            );
  __super::putDataToDoc(doc);
}

void ParametricGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("ParametricCurve"));
  if(doc.findChild(root, _T("common")) != NULL) {
    doc.getValueLF(root, _T("common"), m_commonText);
  } else {
    m_commonText = EMPTYSTRING;
  }
  doc.getValue( root, _T("exprx"     ), m_exprX            );
  doc.getValue( root, _T("expry"     ), m_exprY            );
  getValue(doc, root, _T("interval"  ), m_interval         );
  doc.getValue( root, _T("steps"     ), m_steps            );
  __super::getDataFromDoc(doc);
}
