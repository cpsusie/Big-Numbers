#include "pch.h"
#include <Math/Expression/ExpressionXML.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>


void setValue(XMLDoc &doc, XMLNodePtr n, const ExprParametricR1R3SurfaceParameters &v) {
  setValue(  doc,  n, _T("expr"              ), v.m_expr                           );
  setValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  setValue(  doc,  n                          , (ParametricR1R3SurfaceParameters&)v);
}








void getValue(XMLDoc &doc, XMLNodePtr n, ExprParametricR1R3SurfaceParameters       &v) {
  getValue(  doc,  n, _T("expr"              ), v.m_expr                           );
  getValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  getValue(  doc,  n                          , (ParametricR1R3SurfaceParameters&)v);
}
