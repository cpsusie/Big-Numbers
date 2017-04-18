#include "stdafx.h"
#include <XMLUtil.h>
#include <MFCUtil/2DXML.h>
#include "IsoCurveGraphParameters.h"

IsoCurveGraphParameters::IsoCurveGraphParameters(const String &name, COLORREF color, GraphStyle style, TrigonometricMode trigonomtetricMode) 
: GraphParameters(name,color,0,style) {

  m_trigonometricMode = trigonomtetricMode;
  m_boundingBox       = Rectangle2D(-10,-10,20,20);
  m_cellSize          = 0.1;
}

void IsoCurveGraphParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("IsoCurve"));
  doc.setValue(     root, _T("expr"       ), m_expr             );
  setValue(    doc, root, _T("boundingbox"), m_boundingBox      );
  doc.setValue(     root, _T("cellsize"   ), m_cellSize         );
  setValue(    doc, root,                    m_trigonometricMode);
  setStdValues(doc, root);
}

void IsoCurveGraphParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(         root, _T("IsoCurve"));
  doc.getValueLF(   root, _T("expr"       ), m_expr             );
  getValue(    doc, root, _T("boundingbox"), m_boundingBox      );
  doc.getValue(     root, _T("cellsize"   ), m_cellSize         );
  getValue(    doc, root,                    m_trigonometricMode);
  getStdValues(doc, root);
}
