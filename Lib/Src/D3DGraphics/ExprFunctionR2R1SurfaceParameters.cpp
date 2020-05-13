#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>

void ExprFunctionR2R1SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("Function2DSurface"));
  doc.setValue(    root, _T("expr"       ), m_expr       );
  setValue(doc,    root, _T("xinterval"  ), m_xInterval  );
  setValue(doc,    root, _T("yinterval"  ), m_yInterval  );
  doc.setValue(    root, _T("pointcount" ), m_pointCount );
  doc.setValue(    root, _T("machinecode"), m_machineCode);
  doc.setValue(    root, _T("doublesided"), m_doubleSided);
  setValue(doc,    root, _T("animation"  ), m_animation  );
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr     root = doc.getRoot();
    checkTag(      root, _T("Function2DSurface"));
    doc.getValueLF(root, _T("expr"       ), m_expr       );
    getValue(doc,  root, _T("xinterval"  ), m_xInterval  );
    getValue(doc,  root, _T("yinterval"  ), m_yInterval  );
    doc.getValue(  root, _T("pointcount" ), m_pointCount );
    doc.getValue(  root, _T("machinecode"), m_machineCode);
    doc.getValue(  root, _T("doublesided"), m_doubleSided);
    getValue(doc,  root, _T("animation"  ), m_animation  );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDocOld(XMLDoc &doc) {
  XMLNodePtr     root = doc.getRoot();
  checkTag(      root, _T("Function2DSurface"));
  doc.getValueLF(root, _T("expr"       ), m_expr       );
  getValue(doc,  root, _T("xinterval"  ), m_xInterval  );
  getValue(doc,  root, _T("yinterval"  ), m_yInterval  );
  doc.getValue(  root, _T("pointcount" ), m_pointCount );
  doc.getValue(  root, _T("machinecode"), m_machineCode);
  doc.getValue(  root, _T("doublesided"), m_doubleSided);
  doc.getValue(  root, _T("includetime"), m_animation.m_includeTime);
  if(m_animation.m_includeTime) {
    getValue(doc, root, _T("timeinterval"), m_animation.m_timeInterval);
    doc.getValue( root, _T("framecount"  ), m_animation.m_frameCount  );
  }
}
