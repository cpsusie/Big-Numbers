#include "pch.h"
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>



void setValue(XMLDoc &doc, XMLNodePtr n, const ExprFunctionR2R1SurfaceParameters   &v) {
  setValue(  doc,  n, _T("expr"              ), v.m_expr                           );
  setValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  setValue(  doc,  n                          , (FunctionR2R1SurfaceParameters&)v  );
}








void getValue(XMLDoc &doc, XMLNodePtr n, ExprFunctionR2R1SurfaceParameters         &v) {
  getValueLF(doc,  n, _T("expr"              ), v.m_expr                           );
  getValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  getValue(  doc,  n                          , (FunctionR2R1SurfaceParameters&)v  );
}
