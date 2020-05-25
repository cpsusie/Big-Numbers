#include "stdafx.h"
#include <Math/MathXML.h>
#include "GraphXML.h"
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
