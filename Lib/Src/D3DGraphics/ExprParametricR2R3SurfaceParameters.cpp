#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Expr3 &expr) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(  n, _T("common"            ), expr.getCommonText());
  doc.setValue(  n, _T("exprx"             ), expr.getRawText(0)  );
  doc.setValue(  n, _T("expry"             ), expr.getRawText(1)  );
  doc.setValue(  n, _T("exprz"             ), expr.getRawText(2)  );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Expr3 &expr) {
  XMLNodePtr n = doc.getChild(parent, tag);
  String x, y, z, common;
  doc.getValueLF(n, _T("exprx" ), x     );
  doc.getValueLF(n, _T("expry" ), y     );
  doc.getValueLF(n, _T("exprz" ), z     );
  doc.getValueLF(n, _T("common"), common);
  expr = Expr3(x, y, z, common);
}

void ExprParametricR2R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurface"));
  setValue(doc,  root, _T("expr"              ), m_expr            );
  setValue(doc,  root, _T("tinterval"         ), m_tInterval       );
  setValue(doc,  root, _T("sinterval"         ), m_sInterval       );
  doc.setValue(  root, _T("tstepcount"        ), m_tStepCount      );
  doc.setValue(  root, _T("sstepcount"        ), m_sStepCount      );
  doc.setValue(  root, _T("machinecode"       ), m_machineCode     );
  doc.setValue(  root, _T("doublesided"       ), m_doubleSided     );
  setValue(doc,  root, _T("animation"         ), m_animation       );
}

void ExprParametricR2R3SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr     root = doc.getRoot();
    checkTag(      root, _T("ParametricSurface"));
    getValue(doc,  root, _T("expr"              ), m_expr            );
    getValue( doc, root, _T("tinterval"         ), m_tInterval       );
    getValue( doc, root, _T("sinterval"         ), m_sInterval       );
    doc.getValue(  root, _T("tstepcount"        ), m_tStepCount      );
    doc.getValue(  root, _T("sstepcount"        ), m_sStepCount      );
    doc.getValue(  root, _T("machinecode"       ), m_machineCode     );
    doc.getValue(  root, _T("doublesided"       ), m_doubleSided     );
    getValue(doc,  root, _T("animation"         ), m_animation       );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void ExprParametricR2R3SurfaceParameters::getDataFromDocOld(XMLDoc &doc) {
  String common, exprX, exprY, exprZ;
  XMLNodePtr       root = doc.getRoot();
  checkTag(        root, _T("ParametricSurface"));
  if(doc.findChild(root, _T("common")) != NULL) {
    doc.getValueLF(root, _T("common"), common);
  } else {
    common = EMPTYSTRING;
  }
  doc.getValueLF(  root, _T("exprx"             ), exprX             );
  doc.getValueLF(  root, _T("expry"             ), exprY             );
  doc.getValueLF(  root, _T("exprz"             ), exprZ             );
  m_expr = Expr3(exprX, exprY, exprZ, common);
  getValue( doc,   root, _T("tinterval"         ), m_tInterval       );
  getValue( doc,   root, _T("sinterval"         ), m_sInterval       );
  doc.getValue(    root, _T("tstepcount"        ), m_tStepCount      );
  doc.getValue(    root, _T("sstepcount"        ), m_sStepCount      );
  doc.getValue(    root, _T("machinecode"       ), m_machineCode     );
  doc.getValue(    root, _T("doublesided"       ), m_doubleSided     );
  doc.getValue(    root, _T("includetime"       ), m_animation.m_includeTime     );
  if(m_animation.m_includeTime) {
    getValue(doc,  root, _T("timeinterval"      ), m_animation.m_timeInterval    );
    doc.getValue(  root, _T("framecount"        ), m_animation.m_frameCount      );
  }
}
