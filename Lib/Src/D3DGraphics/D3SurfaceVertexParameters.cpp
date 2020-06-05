#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/D3SurfaceVertexParameters.h>

D3SurfaceVertexParameters::D3SurfaceVertexParameters() {
  m_hasNormals = true;
  m_hasTexture = false;
}

DWORD D3SurfaceVertexParameters::getFVF() const {
  DWORD fvf = D3DFVF_XYZ;
  if(m_hasNormals) fvf |= D3DFVF_NORMAL;
  if(m_hasTexture) fvf |= D3DFVF_TEX1;
  return fvf;
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3SurfaceVertexParameters &v) {
  setValue(doc, n, _T("hasNormals"     ), v.m_hasNormals     );
  setValue(doc, n, _T("hasTexture"     ), v.m_hasTexture     );
  setValue(doc, n, _T("texturefilename"), v.m_textureFileName);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3SurfaceVertexParameters &v) {
  getValue(doc, n, _T("hasNormals"     ), v.m_hasNormals     );
  getValue(doc, n, _T("hasTexture"     ), v.m_hasTexture     );
  getValue(doc, n, _T("texturefilename"), v.m_textureFileName);
}
