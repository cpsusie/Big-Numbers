#include "stdafx.h"
#include <XMLUtil.h>
#include "ParametricGraph.h"

ParametricGraphParameters::ParametricGraphParameters(const String &name, COLORREF color, const RollingAvg &rollingAvg, GraphStyle style, TrigonometricMode trigonometricMode)
: ExprGraphParameters(name,color,rollingAvg,style, trigonometricMode) {
  m_interval          = DoubleInterval(0,1);
  m_steps             = 500;
}

void ParametricGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr      root = doc.createRoot(_T("ParametricCurve")  );
  setValue(  doc, root, _T("common"    ), m_commonText         );
  setValue(  doc, root, _T("exprx"     ), m_exprX              );
  setValue(  doc, root, _T("expry"     ), m_exprY              );
  setValue(  doc, root, _T("interval"  ), m_interval           );
  setValue(  doc, root, _T("steps"     ), m_steps              );
  __super::putDataToDoc(doc);
}

void ParametricGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("ParametricCurve"));
  getValueLF(doc, root, _T("common"), m_commonText, EMPTYSTRING);
  getValue(  doc, root, _T("exprx"     ), m_exprX              );
  getValue(  doc, root, _T("expry"     ), m_exprY              );
  getValue(  doc, root, _T("interval"  ), m_interval           );
  getValue(  doc, root, _T("steps"     ), m_steps              );
  __super::getDataFromDoc(doc);
}
