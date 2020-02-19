#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/Light.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/Profile.h>

D3LightControlDirectional::D3LightControlDirectional(D3Scene &scene, int lightIndex) : D3LightControl(scene, lightIndex) {
  createMaterial();
  setSize(0.4f);
  setSphereRadius(2);
  setName(format(_T("Directional light (%d)"), lightIndex));
  m_mesh = createArrowMesh();
}

static const Point2D dirLightMeshProfilePoints[] = {
  Point2D( 0   , 0   )
 ,Point2D(-0.47, 0.25)
 ,Point2D(-0.47, 0.1 )
 ,Point2D(-1   , 0.1 )
 ,Point2D(-1   ,   0 )
};

LPD3DXMESH D3LightControlDirectional::createArrowMesh() {
  Profile prof;
  prof.addLineStrip(dirLightMeshProfilePoints, ARRAYSIZE(dirLightMeshProfilePoints));
  ProfileRotationParameters param;
  param.m_alignx     = 0;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = ROTATESMOOTH;
  param.m_rotateAxis = 0;
  AbstractMeshFactory &factory = getScene();
  LPD3DXMESH mesh = rotateProfile(factory, prof, param, false);

  return optimizeMesh(mesh);
}

D3DXMATRIX &D3LightControlDirectional::getWorld() {
  const LIGHT light = getLight();
  const float size  = getSize();
  m_world = D3World().setScaleAll(size).setOrientation(createOrientation(light.Direction)).setPos(-m_sphereRadius * light.Direction);
  return __super::getWorld();
}
