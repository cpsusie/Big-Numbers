#include "pch.h"
#include <D3DGraphics/ExprIsoSurfaceParameters.h>
#include <D3DGraphics/D3XML.h>



void setValue(XMLDoc &doc, XMLNodePtr n, const ExprIsoSurfaceParameters            &v) {
  setValue(  doc,  n, _T("expr"              ), v.m_expr                           );
  setValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  setValue(  doc,  n                          , (IsoSurfaceParameters&)v           );
}








void getValue(XMLDoc &doc, XMLNodePtr n, ExprIsoSurfaceParameters                  &v) {
  getValueLF(doc,  n, _T("expr"              ), v.m_expr                           );
  getValue(  doc,  n, _T("machinecode"       ), v.m_machineCode                    );
  getValue(  doc,  n                          , (IsoSurfaceParameters&)v           );
}
