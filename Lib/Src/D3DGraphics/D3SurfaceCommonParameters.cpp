#include "pch.h"
#include <D3DGraphics/D3SurfaceCommonParameters.h>

D3SurfaceCommonParameters::D3SurfaceCommonParameters() {
  m_calculateNormals  = true;
  m_calculateTexture  = false;
  m_doubleSided       = false;
  m_animation.reset();
}

DWORD D3SurfaceCommonParameters::getFVF() const {
  DWORD fvf = D3DFVF_XYZ;
  if(m_calculateNormals) fvf |= D3DFVF_NORMAL;
  if(m_calculateTexture) fvf |= D3DFVF_TEX1;
  return fvf;
}
