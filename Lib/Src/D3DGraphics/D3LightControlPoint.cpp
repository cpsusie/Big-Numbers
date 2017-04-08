#include "pch.h"
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/MeshCreators.h>

D3LightControlPoint::D3LightControlPoint(D3Scene &scene, int lightIndex) 
: D3LightControl(scene, lightIndex)
{
  setSize(0.08f);
  setName(format(_T("Point light (%d)"), lightIndex));
  m_mesh = createSphereMesh(getDevice(), 1);
  optimizeMesh(m_mesh);
}

D3DXMATRIX D3LightControlPoint::getWorldMatrix() const {
  return D3PosDirUpScale()
           .setScaleAll(getSize())
           .setPos(getLightParam().Position)
           .getWorldMatrix();
}
