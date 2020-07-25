#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>

void ExprIsoSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("IsoSurface"));
  setValue(doc,    root, *this);
}

void ExprIsoSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("IsoSurface"));
  getValue(doc,    root, *this);
}
