#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Light.h>
#include <D3DGraphics/D3LightControl.h>

D3LightControlDirectional::D3LightControlDirectional(D3Scene &scene, int lightIndex) : D3LightControl(scene, lightIndex) {
  createMaterial();
  setSize(0.4f);
  setSphereRadius(2);
  setName(format(_T("Directional light (%d)"), lightIndex));
}

D3DXMATRIX &D3LightControlDirectional::getWorld() {
  const D3Light light = getLight();
  const float   size  = getSize();
  return m_world = D3World().setScaleAll(size).setOrientation(createOrientation(light.Direction)).setPos(-m_sphereRadius * light.Direction);
}
