#include "stdafx.h"
#include "ExprGraphParameters.h"

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

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode trigoMode) {
  String str = toLowerCase(toString(trigoMode));
  doc.setValue(n, _T("trigo"), str);
}

void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &trigoMode) {
  String str;
  doc.getValue(n, _T("trigo"), str);
  trigoMode = getTrigonometricModeFromString(str);
}

