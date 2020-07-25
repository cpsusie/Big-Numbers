#include "pch.h"
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>

void ExprFunctionR2R1SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("Function2DSurface"));
  setValue( doc, root, *this);
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  XMLDoc::checkTag(root, _T("Function2DSurface"));
  getValue(doc,  root, *this);
}
