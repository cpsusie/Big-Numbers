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

static void setValue(XMLDoc &doc, XMLNodePtr n, const EquationAttributes &attr) {
  setValue(doc, n, _T("start"  ), attr.getStartValue());
  setValue(doc, n, _T("color"  ), attr.getColor(), true);
  setValue(doc, n, _T("visible"), attr.isVisible()   );
}

static void getValue(XMLDoc &doc, XMLNodePtr n, EquationAttributes &attr) {
  Real     startValue;
  COLORREF color;
  bool     visible;
  getValue(doc, n, _T("start"  ), startValue );
  getValue(doc, n, _T("color"  ), color, true);
  getValue(doc, n, _T("visible"), visible    );
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
  setValue(doc, root, _T("eps"      ), m_maxError                 );
  setValue(doc, root                 , getGraphStyle()            );
  setValue(doc, root                 , getTrigonometricMode()     );
  setValue(doc, root, _T("dim"      ), dim                        );
  setValue(doc, root, _T("common"   ), eqDescArray.getCommonText());

  XMLNodePtr eqListNode = doc.createNode(root,_T("equations"));
  for(UINT i = 0; i < dim; i++) {
    const DiffEquationDescription &desc = eqDescArray[i];
    const EquationAttributes      &attr = m_attrArray[i];
    XMLNodePtr eq = doc.createNode(eqListNode, format(_T("eq%u"),i).cstr());
    setValue(doc, eq, _T("name"), desc.getName()    );
    setValue(doc, eq, _T("expr"), desc.getExprText());
    setValue(doc, eq, _T("attr"), attr              );
  }
}

void DiffEquationGraphParameters::getDataFromDoc(XMLDoc &doc) {
  UINT              dim;
  GraphStyle        graphStyle;
  TrigonometricMode trigonometricMode;
  String            commonText;

  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("DiffEquation"));
  getValue(  doc, root, _T("interval" ), m_interval         );
  getValue(  doc, root, _T("eps"      ), m_maxError         );
  getValue(  doc, root                 , graphStyle         );
  getValue(  doc, root                 , trigonometricMode  );
  getValue(  doc, root, _T("dim"      ), dim                );
  getValueLF(doc, root, _T("common"), commonText, EMPTYSTRING);

  setGraphStyle(       graphStyle);
  setTrigonometricMode(trigonometricMode);
  m_equationDescArray.setCommonText(commonText);

  removeAllEquations();
  XMLNodePtr eqListNode = doc.getChild(root, _T("equations"));
  for(UINT i = 0; i < dim; i++) {
    XMLNodePtr         eq = doc.getChild(eqListNode, format(_T("eq%u"),i).cstr());
    String             name, expr;
    EquationAttributes attr;
    getValue(  doc, eq, _T("name"), name);
    getValueLF(doc, eq, _T("expr"), expr);
    getValue(  doc, eq, _T("attr"), attr);
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
