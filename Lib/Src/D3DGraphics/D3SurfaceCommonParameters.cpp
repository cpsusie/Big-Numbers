#include "pch.h"
#include <D3DGraphics/D3SurfaceCommonParameters.h>

D3SurfaceCommonParameters::D3SurfaceCommonParameters() {
  m_hasNormals  = true;
  m_hasTexture  = false;
  m_doubleSided = false;
  m_animation.reset();
}

DWORD D3SurfaceCommonParameters::getFVF() const {
  DWORD fvf = D3DFVF_XYZ;
  if(m_hasNormals) fvf |= D3DFVF_NORMAL;
  if(m_hasTexture) fvf |= D3DFVF_TEX1;
  return fvf;
}
