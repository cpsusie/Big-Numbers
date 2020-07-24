#include "pch.h"
#include <Math/Expression/ExpressionXML.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

void ExprParametricR2R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurface"));
  setValue(doc,  root, _T("expr"              ), m_expr            );
  setValue(doc,  root, _T("tinterval"         ), m_tInterval       );
  setValue(doc,  root, _T("sinterval"         ), m_sInterval       );
  setValue(doc,  root, _T("tstepcount"        ), m_tStepCount      );
  setValue(doc,  root, _T("sstepcount"        ), m_sStepCount      );
  setValue(doc,  root, _T("machinecode"       ), m_machineCode     );
  setValue(doc,  root, _T("common"            ), *(D3SurfaceCommonParameters*)this);
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
    getValue(doc,    root, _T("common"            ), *(D3SurfaceCommonParameters*)this);
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
}
