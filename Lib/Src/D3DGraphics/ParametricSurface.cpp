#include "pch.h"
#include <XMLUtil.h>
#include <D3DGraphics/ParametricSurface.h>

ParametricSurfaceParameters::ParametricSurfaceParameters() {
  m_tInterval     = DoubleInterval(-10,10);
  m_sInterval     = DoubleInterval(-10,10);
  m_tStepCount    = 10;
  m_sStepCount    = 10;
  m_machineCode   = true;
  m_doubleSided   = true;
  m_includeTime   = false;
  m_timeInterval  = DoubleInterval(0,10);
  m_frameCount    = 20;
}

void ParametricSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("ParametricSurface"));
  doc.setValue(  root, _T("common"            ), m_commonText      );
  doc.setValue(  root, _T("exprx"             ), m_exprX           );
  doc.setValue(  root, _T("expry"             ), m_exprY           );
  doc.setValue(  root, _T("exprz"             ), m_exprZ           );
  setValue(doc,  root, _T("tinterval"         ), m_tInterval       );
  setValue(doc,  root, _T("sinterval"         ), m_sInterval       );
  doc.setValue(  root, _T("tstepcount"        ), m_tStepCount      );
  doc.setValue(  root, _T("sstepcount"        ), m_sStepCount      );
  doc.setValue(  root, _T("machinecode"       ), m_machineCode     );
  doc.setValue(  root, _T("doublesided"       ), m_doubleSided     );
  doc.setValue(  root, _T("includetime"       ), m_includeTime     );
  if(m_includeTime) {
    setValue(doc, root, _T("timeinterval"     ), m_timeInterval    );
    doc.setValue( root, _T("framecount"       ), m_frameCount      );
  }
}

void ParametricSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("ParametricSurface"));
  if(doc.findChild(root, _T("common")) != NULL) {
    doc.getValueLF(root, _T("common"), m_commonText);
  } else {
    m_commonText = EMPTYSTRING;
  }
  doc.getValueLF(root, _T("exprx"             ), m_exprX           );
  doc.getValueLF(root, _T("expry"             ), m_exprY           );
  doc.getValueLF(root, _T("exprz"             ), m_exprZ           );
  getValue( doc, root, _T("tinterval"         ), m_tInterval       );
  getValue( doc, root, _T("sinterval"         ), m_sInterval       );
  doc.getValue(  root, _T("tstepcount"        ), m_tStepCount      );
  doc.getValue(  root, _T("sstepcount"        ), m_sStepCount      );
  doc.getValue(  root, _T("machinecode"       ), m_machineCode     );
  doc.getValue(  root, _T("doublesided"       ), m_doubleSided     );
  doc.getValue(  root, _T("includetime"       ), m_includeTime     );
  if(m_includeTime) {
    getValue(doc, root, _T("timeinterval"     ), m_timeInterval    );
    doc.getValue( root, _T("framecount"       ), m_frameCount      );
  }
}
