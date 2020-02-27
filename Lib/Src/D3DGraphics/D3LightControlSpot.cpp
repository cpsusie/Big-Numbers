#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/Profile.h>

D3LightControlSpot::D3LightControlSpot(D3Scene &scene, int lightIndex)
: D3LightControl(scene, lightIndex)
{
  createMaterial();
  setSize(0.4f);
  setName(format(_T("Spot light (%d)"), lightIndex));
}

D3DXMATRIX &D3LightControlSpot::getWorld() {
  const D3Light light = getLight();
  const float   size  = getSize();
  return m_world = D3World().setScaleAll(size).setOrientation(createOrientation(light.Direction)).setPos(light.Position);
}
