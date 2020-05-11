#include "pch.h"
#include <XMLUtil.h>
#include <D3DGraphics/ExprFunction2DSurfaceParameters.h>

void ExprFunction2DSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("Function2DSurface"));
  doc.setValue( root, _T("expr"       ), m_expr       );
  setValue(doc, root, _T("xinterval"  ), m_xInterval  );
  setValue(doc, root, _T("yinterval"  ), m_yInterval  );
  doc.setValue( root, _T("pointcount" ), m_pointCount );
  doc.setValue( root, _T("machinecode"), m_machineCode);
  doc.setValue( root, _T("doublesided"), m_doubleSided);
  doc.setValue( root, _T("includetime"), m_includeTime);
  if(m_includeTime) {
    setValue(doc, root, _T("timeinterval"), m_timeInterval);
    doc.setValue( root, _T("framecount"), m_frameCount);
  }
}

void ExprFunction2DSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("Function2DSurface"));
  doc.getValueLF(root, _T("expr"       ), m_expr       );
  getValue(doc,  root, _T("xinterval"  ), m_xInterval  );
  getValue(doc,  root, _T("yinterval"  ), m_yInterval  );
  doc.getValue(  root, _T("pointcount" ), m_pointCount );
  doc.getValue(  root, _T("machinecode"), m_machineCode);
  doc.getValue(  root, _T("doublesided"), m_doubleSided);
  doc.getValue(  root, _T("includetime"), m_includeTime);
  if(m_includeTime) {
    getValue(doc, root, _T("timeinterval"), m_timeInterval);
    doc.getValue( root, _T("framecount"  ), m_frameCount  );
  }
}
