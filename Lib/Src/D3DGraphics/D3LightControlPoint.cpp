#include "pch.h"
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/MeshCreators.h>

D3LightControlPoint::D3LightControlPoint(D3Scene &scene, int lightIndex)
: D3LightControl(scene, lightIndex)
{
  createMaterial();
  setSize(0.08f);
  setName(format(_T("Point light (%d)"), lightIndex));
  m_mesh = createSphereMesh(getScene(), 1);
  optimizeMesh(m_mesh);
}

D3PosDirUpScale &D3LightControlPoint::getPDUS() {
  return m_pdus.setScaleAll(getSize())
               .setPos(getLight().Position);
}
