#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>

void ExprParametricR1R3SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr     root = doc.createRoot(_T("ParametricSurfaceR1R3"));
  setValue(doc,  root, *this);
}

void ExprParametricR1R3SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("ParametricSurfaceR1R3"));
  getValue(doc,    root, *this);
}
