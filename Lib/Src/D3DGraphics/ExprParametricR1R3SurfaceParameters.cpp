#include "pch.h"
#include <Math/Expression/ExpressionXML.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>

void ExprParametricR1R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurface"));
  setValue(doc,  root, _T("expr"              ), m_expr            );
  setValue(doc,  root, _T("tinterval"         ), m_tInterval       );
  setValue(doc,  root, _T("tstepcount"        ), m_tStepCount      );
  setValue(doc,  root, _T("machinecode"       ), m_machineCode     );
  setValue(doc,  root, _T("common"            ), *(D3SurfaceCommonParameters*)this);
}

void ExprParametricR1R3SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("ParametricSurface"));
  getValue(doc,    root, _T("expr"              ), m_expr            );
  getValue(doc,    root, _T("tinterval"         ), m_tInterval       );
  getValue(doc,    root, _T("tstepcount"        ), m_tStepCount      );
  getValue(doc,    root, _T("machinecode"       ), m_machineCode     );
  getValue(doc,    root, _T("common"            ), *(D3SurfaceCommonParameters*)this);
}
