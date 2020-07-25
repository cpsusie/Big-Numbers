#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>

void ExprParametricR2R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurfaceR2R3"));
  setValue(doc,  root, *this);
}

void ExprParametricR2R3SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("ParametricSurfaceR2R3"));
  getValue(doc,    root, *this);
}
