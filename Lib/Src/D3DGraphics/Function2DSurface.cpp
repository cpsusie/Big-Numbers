#include "pch.h"
#include <XMLUtil.h>
#include <D3DGraphics/Function2DSurface.h>

Function2DSurfaceParameters::Function2DSurfaceParameters() {
  m_xInterval     = DoubleInterval(-10,10);
  m_yInterval     = DoubleInterval(-10,10);
  m_pointCount    = 10;
  m_machineCode   = true;
  m_doubleSided   = true;
  m_includeTime   = false;
  m_timeInterval  = DoubleInterval(0,10);
  m_frameCount    = 20;
}

void Function2DSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("Function2DSurface"));
  doc.setValue( root, _T("expr"       ), m_expr       );
  setValue(doc, root, _T("xinterval"  ), m_xInterval  );
  setValue(doc, root, _T("yinterval"  ), m_yInterval  );
  doc.setValue( root, _T("pointcount" ), m_pointCount );
  doc.setValue( root, _T("machinecode"), m_machineCode);
  doc.setValue( root, _T("doublesided"), m_doubleSided);
  doc.setValue( root, _T("includetime"), m_includeTime);
  if (m_includeTime) {
    setValue(doc, root, _T("timeinterval"), m_timeInterval);
    doc.setValue( root, _T("framecount"), m_frameCount);
  }
}

void Function2DSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
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
