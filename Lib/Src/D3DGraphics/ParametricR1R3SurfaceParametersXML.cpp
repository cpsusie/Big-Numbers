#include "pch.h"
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/ParametricR1R3SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>


void setValue(XMLDoc &doc, XMLNodePtr n, const ParametricR1R3SurfaceParameters     &v) {
  setValue(  doc,  n, _T("tinterval"         ), v.m_tInterval                      );
  setValue(  doc,  n, _T("tstepcount"        ), v.m_tStepCount                     );
  setValue(  doc,  n, _T("profilename"       ), v.m_profileFileName                );
  setValue(  doc,  n, _T("smoothnormals"     ), v.m_smoothNormals                  );
  setValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
}








void getValue(XMLDoc &doc, XMLNodePtr n, ParametricR1R3SurfaceParameters           &v) {
  getValue(  doc,  n, _T("tinterval"         ), v.m_tInterval                      );
  getValue(  doc,  n, _T("tstepcount"        ), v.m_tStepCount                     );
  getValue(  doc,  n, _T("profilename"       ), v.m_profileFileName                );
  getValue(  doc,  n, _T("smoothnormals"     ), v.m_smoothNormals                  );
  getValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
}
