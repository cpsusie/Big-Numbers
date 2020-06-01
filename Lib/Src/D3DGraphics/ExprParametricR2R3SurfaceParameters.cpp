#include "pch.h"
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Expr3 &expr) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("common"            ), expr.getCommonText());
  setValue(doc, n, _T("exprx"             ), expr.getRawText(0)  );
  setValue(doc, n, _T("expry"             ), expr.getRawText(1)  );
  setValue(doc, n, _T("exprz"             ), expr.getRawText(2)  );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, Expr3 &expr) {
  XMLNodePtr n = doc.getChild(parent, tag);
  String x, y, z, common;
  getValueLF(doc,n, _T("exprx" ), x     );
  getValueLF(doc,n, _T("expry" ), y     );
  getValueLF(doc,n, _T("exprz" ), z     );
  getValueLF(doc,n, _T("common"), common);
  expr = Expr3(x, y, z, common);
}

void ExprParametricR2R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurface"));
  setValue(doc,  root, _T("expr"              ), m_expr            );
  setValue(doc,  root, _T("tinterval"         ), m_tInterval       );
  setValue(doc,  root, _T("sinterval"         ), m_sInterval       );
  setValue(doc,  root, _T("tstepcount"        ), m_tStepCount      );
  setValue(doc,  root, _T("sstepcount"        ), m_sStepCount      );
  setValue(doc,  root, _T("machinecode"       ), m_machineCode     );
  setValue(doc,  root, _T("doublesided"       ), m_doubleSided     );
  setValue(doc,  root, _T("animation"         ), m_animation       );
}

void ExprParametricR2R3SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr       root = doc.getRoot();
    XMLDoc::checkTag(root, _T("ParametricSurface"));
    getValue(doc,    root, _T("expr"              ), m_expr            );
    getValue(doc,    root, _T("tinterval"         ), m_tInterval       );
    getValue(doc,    root, _T("sinterval"         ), m_sInterval       );
    getValue(doc,    root, _T("tstepcount"        ), m_tStepCount      );
    getValue(doc,    root, _T("sstepcount"        ), m_sStepCount      );
    getValue(doc,    root, _T("machinecode"       ), m_machineCode     );
    getValue(doc,    root, _T("doublesided"       ), m_doubleSided     );
    getValue(doc,    root, _T("animation"         ), m_animation       );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void ExprParametricR2R3SurfaceParameters::getDataFromDocOld(XMLDoc &doc) {
  String common, exprX, exprY, exprZ;
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("ParametricSurface"));
  getValueLF(doc,  root, _T("common"            ), common,  EMPTYSTRING);
  getValueLF(doc,  root, _T("exprx"             ), exprX               );
  getValueLF(doc,  root, _T("expry"             ), exprY               );
  getValueLF(doc,  root, _T("exprz"             ), exprZ               );
  m_expr = Expr3(exprX, exprY, exprZ, common);
  getValue(doc,  root, _T("tinterval"         ), m_tInterval           );
  getValue(doc,  root, _T("sinterval"         ), m_sInterval           );
  getValue(doc,  root, _T("tstepcount"        ), m_tStepCount          );
  getValue(doc,  root, _T("sstepcount"        ), m_sStepCount          );
  getValue(doc,  root, _T("machinecode"       ), m_machineCode         );
  getValue(doc,  root, _T("doublesided"       ), m_doubleSided         );
  getValue(doc,  root, _T("includetime"       ), m_animation.m_includeTime     );
  if(m_animation.m_includeTime) {
    getValue(doc,  root, _T("timeinterval"      ), m_animation.m_timeInterval  );
    getValue(doc,  root, _T("framecount"        ), m_animation.m_frameCount    );
  }
}
