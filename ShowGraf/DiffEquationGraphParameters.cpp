#include "stdafx.h"
#include <XMLUtil.h>
#include <Math/MathXML.h>
#include "GraphXML.h"
#include "DiffEquationGraph.h"

DiffEquationGraphParameters::DiffEquationGraphParameters(const String &name, GraphStyle style, TrigonometricMode trigonometricMode)
  : ExprGraphParameters(name, 0, RollingAvg::s_default, style,trigonometricMode) {
  m_interval          = DoubleInterval(0, 1);
  m_maxError          = 0.1;
}

static void setValue(XMLDoc &doc, XMLNodePtr parent, const EquationAttributes &attr) {
  XMLNodePtr n = doc.createNode(parent, _T("attr"));
  doc.setValue(n, _T("start"  ), attr.getStartValue());
  doc.setValue(n, _T("color"  ), format(_T("%08x"), attr.getColor()));
  doc.setValue(n, _T("visible"), attr.isVisible()   );
}

static void getValue(XMLDoc &doc, XMLNodePtr parent, EquationAttributes &attr) {
  XMLNodePtr n = doc.getChild(parent, _T("attr"));
  Real     startValue;
  COLORREF color;
  bool     visible;
  doc.getValue(n, _T("start"  ), startValue);
  String str;
  doc.getValue(n, _T("color"  ), str       );
  _stscanf(str.cstr(), _T("%x"), &color    );
  doc.getValue(n, _T("visible"), visible   );
  attr.setStartValue(startValue);
  attr.setColor(     color     );
  attr.setVisible(   visible   );
}

void DiffEquationGraphParameters::putDataToDoc(XMLDoc &doc) {
  const DiffEquationDescriptionArray &eqDescArray = m_equationDescArray;
  const UINT dim = (UINT)eqDescArray.size();

  assert(dim == m_attrArray.size());

  XMLNodePtr root = doc.createRoot(_T("DiffEquation"));
  setValue(doc, root, _T("interval" ), m_interval                 );
  doc.setValue( root, _T("eps"      ), m_maxError                 );
  setValue(doc, root                 , getGraphStyle()            );
  setValue(doc, root                 , getTrigonometricMode()     );
  doc.setValue( root, _T("dim"      ), dim                        );
  doc.setValue( root, _T("common"   ), eqDescArray.getCommonText());

  XMLNodePtr eqListNode = doc.createNode(root,_T("equations"));
  for(UINT i = 0; i < dim; i++) {
    const DiffEquationDescription &desc = eqDescArray[i];
    const EquationAttributes      &attr = m_attrArray[i];
    XMLNodePtr eq = doc.createNode(eqListNode, format(_T("eq%u"),i).cstr());
    doc.setValue( eq, _T("name"), desc.getName()    );
    doc.setValue( eq, _T("expr"), desc.getExprText());
    setValue(doc, eq            , attr       );
  }
}

void DiffEquationGraphParameters::getDataFromDoc(XMLDoc &doc) {
  UINT dim;
  XMLNodePtr root = doc.getRoot();
  XMLDoc::checkTag(     root, _T("DiffEquation"));
  getValue(doc, root, _T("interval" ), m_interval         );
  doc.getValue( root, _T("eps"      ), m_maxError         );
  GraphStyle graphStyle;
  getValue(doc, root                 , graphStyle         );
  setGraphStyle(graphStyle);
  TrigonometricMode trigonometricMode;
  getValue(doc, root                 , trigonometricMode  );
  setTrigonometricMode(trigonometricMode);
  doc.getValue( root, _T("dim"      ), dim                );
  String commonText;
  if(doc.findChild(root, _T("common")) != NULL) {
    doc.getValueLF(root, _T("common"), commonText);
  }
  m_equationDescArray.setCommonText(commonText);

  removeAllEquations();
  XMLNodePtr eqListNode = doc.getChild(root, _T("equations"));
  for(UINT i = 0; i < dim; i++) {
    XMLNodePtr eq = doc.getChild(eqListNode, format(_T("eq%u"),i).cstr());
    String name, expr;
    doc.getValue(  eq, _T("name"), name);
    doc.getValueLF(eq, _T("expr"), expr);
    EquationAttributes      attr;
    getValue( doc, eq     , attr       );
    addEquation(DiffEquationDescription(name,expr), attr);
  }
}

DiffEquationSet DiffEquationGraphParameters::getVisibleEquationSet() const {
  DiffEquationSet result;
  for(size_t i = 0; i < m_attrArray.size(); i++) {
    if(m_attrArray[i].isVisible()) result.add((UINT)i);
  }
  return result;
}

Vector DiffEquationGraphParameters::getStartVector() const {
  const int vectorDim = getEquationCount() + 1;
  Vector result(vectorDim);
  result[0] = m_interval.getFrom();
  for(size_t i = 1; i < result.getDimension(); i++) {
    result[i] = m_attrArray[i-1].getStartValue();
  }
  return result;
}

String EquationAttributes::toString() const {
  return format(_T("%s %s %08x"), ::toString(m_startValue).cstr(), boolToStr(m_visible), m_color);
}
