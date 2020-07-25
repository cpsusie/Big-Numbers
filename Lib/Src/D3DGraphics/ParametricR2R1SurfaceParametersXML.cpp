#include "pch.h"
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>


void setValue(XMLDoc &doc, XMLNodePtr n, const FunctionR2R1SurfaceParameters       &v) {
  setValue(  doc,  n, _T("xinterval"         ), v.m_xInterval                      );
  setValue(  doc,  n, _T("yinterval"         ), v.m_yInterval                      );
  setValue(  doc,  n, _T("pointcount"        ), v.m_pointCount                     );
  setValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
}







void getValue(XMLDoc &doc, XMLNodePtr n, FunctionR2R1SurfaceParameters             &v) {
  getValue(  doc,  n, _T("xinterval"         ), v.m_xInterval                      );
  getValue(  doc,  n, _T("yinterval"         ), v.m_yInterval                      );
  getValue(  doc,  n, _T("pointcount"        ), v.m_pointCount                     );
  getValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
}
