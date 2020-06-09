#include "pch.h"
#include <D3DGraphics/D3SurfaceCommonParameters.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3SurfaceCommonParameters &v) {
  setValue(doc, n, _T("hasNormals"     ), v.m_hasNormals     );
  setValue(doc, n, _T("hasTexture"     ), v.m_hasTexture     );
  setValue(doc, n, _T("texturefilename"), v.m_textureFileName);
  setValue(doc, n, _T("doublesided"    ), v.m_doubleSided     );
  setValue(doc, n, _T("animation"      ), v.m_animation       );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3SurfaceCommonParameters &v) {
  getValue(doc, n, _T("hasNormals"     ), v.m_hasNormals     );
  getValue(doc, n, _T("hasTexture"     ), v.m_hasTexture     );
  getValue(doc, n, _T("texturefilename"), v.m_textureFileName);
  getValue(doc, n, _T("doublesided"    ), v.m_doubleSided     );
  getValue(doc, n, _T("animation"      ), v.m_animation       );
}
