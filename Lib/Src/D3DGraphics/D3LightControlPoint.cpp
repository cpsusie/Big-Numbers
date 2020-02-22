#include "pch.h"
#include <D3DGraphics/D3Scene.h>
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

D3DXMATRIX &D3LightControlPoint::getWorld() {
  const D3Light light = getLight();
  const float   size  = getSize();
  return m_world = D3World().setScaleAll(size).setPos(light.Position);
}
