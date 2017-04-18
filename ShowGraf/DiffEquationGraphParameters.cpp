#include "stdafx.h"
#include <XMLUtil.h>
#include "DiffEquationGraph.h"

DiffEquationGraphParameters::DiffEquationGraphParameters(const String &name, GraphStyle style, TrigonometricMode trigonomtetricMode)
  : GraphParameters(name, 0, 1, style) {

  m_trigonometricMode = trigonomtetricMode;
  m_interval          = DoubleInterval(0, 1);
  m_eps               = 0.1;
}

static void setValue(XMLDoc &doc, XMLNodePtr parent, const EquationAttributes &attr) {
  XMLNodePtr n = doc.createNode(parent, _T("attr"));
  doc.setValue(n, _T("start"  ), attr.m_startValue);
  doc.setValue(n, _T("color"  ), format(_T("%08x"), attr.m_color));
  doc.setValue(n, _T("visible"), attr.m_visible   );
}

static void getValue(XMLDoc &doc, XMLNodePtr parent, EquationAttributes &attr) {
  XMLNodePtr n = PersistentParameter::getChild(doc, parent, _T("attr"));
  doc.getValue(n, _T("start"  ), attr.m_startValue);
  String str;
  doc.getValue(n, _T("color"  ), str              );
  _stscanf(str.cstr(), _T("%x"), &attr.m_color    );
  doc.getValue(n, _T("visible"), attr.m_visible   );
}

void DiffEquationGraphParameters::putDataToDoc(XMLDoc &doc) {
  const DiffEquationSystemDescription &eqDescArray = m_equationsDescription;
  const UINT dim = (UINT)eqDescArray.size();
  
  assert(dim == m_attrArray.size());

  XMLNodePtr root = doc.createRoot(_T("DiffEquation"));
  setValue(doc, root, _T("interval" ), m_interval         );
  doc.setValue( root, _T("eps"      ), m_eps              );
  setValue(doc, root                 , m_style            );
  setValue(doc, root                 , m_trigonometricMode);
  doc.setValue( root, _T("dim"      ), dim                );

  XMLNodePtr eqListNode = doc.createNode(root,_T("equations"));
  for (UINT i = 0; i < dim; i++) {
    const DiffEquationDescription &desc = eqDescArray[i];
    const EquationAttributes      &attr = m_attrArray[i];
    XMLNodePtr eq = doc.createNode(eqListNode, format(_T("eq%d"),i).cstr());
    doc.setValue( eq, _T("name"), desc.m_name);
    doc.setValue( eq, _T("expr"), desc.m_expr);
    setValue(doc, eq            , attr       );
  }
}

void DiffEquationGraphParameters::getDataFromDoc(XMLDoc &doc) {
  int dim;
  XMLNodePtr root = doc.getRoot();
  checkTag(     root, _T("DiffEquation"));
  getValue(doc, root, _T("interval" ), m_interval         );
  doc.getValue( root, _T("eps"      ), m_eps              );
  getValue(doc, root                 , m_style            );
  getValue(doc, root                 , m_trigonometricMode);
  doc.getValue( root, _T("dim"      ), dim                );

  XMLNodePtr eqListNode = getChild(doc, root, _T("equations"));
  for (int i = 0; i < dim; i++) {
    XMLNodePtr eq = getChild(doc, eqListNode, format(_T("eq%d"),i).cstr());
    DiffEquationDescription desc;
    EquationAttributes      attr;
    doc.getValue(  eq, _T("name"), desc.m_name);
    doc.getValueLF(eq, _T("expr"), desc.m_expr);
    getValue( doc, eq            , attr       );
    m_equationsDescription.add(desc);
    m_attrArray.add(attr);
  }
}

DiffEquationSet DiffEquationGraphParameters::getVisibleEquationSet() const {
  DiffEquationSet result;
  for (size_t i = 0; i < m_attrArray.size(); i++) {
    const EquationAttributes &attr = m_attrArray[i];
    if(attr.m_visible) result.add((UINT)i);
  }
  return result;
}

Vector DiffEquationGraphParameters::getStartVector() const {
  const int vectorDim = getEquationCount() + 1;
  Vector result(vectorDim);
  result[0] = m_interval.getFrom();
  for (size_t i = 1; i < result.getDimension(); i++) {
    result[i] = m_attrArray[i-1].m_startValue;
  }
  return result;
}

String EquationAttributes::toString() const {
  return format(_T("%s %s %08x"), ::toString(m_startValue).cstr(), boolToStr(m_visible), m_color);
}
