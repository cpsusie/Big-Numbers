#include "stdafx.h"
#include "D3LightControl.h"

D3LightControlDirectional::D3LightControlDirectional(D3Scene &scene, int lightIndex) : D3LightControl(scene, lightIndex) {
  setSize(0.4f);
  setSphereRadius(2);
  setName(format(_T("Directional light (%d)"), lightIndex));
  m_mesh = createArrowMesh();
}

static const Point2D dirLightMeshProfilePoints[] = {
  Point2D(1   ,   0 ) 
 ,Point2D(1   , 0.1 ) 
 ,Point2D(0.47, 0.1 ) 
 ,Point2D(0.47, 0.25)
 ,Point2D(0   , 0   )
};

LPD3DXMESH D3LightControlDirectional::createArrowMesh() {
  Profile prof;
  prof.addLineStrip(dirLightMeshProfilePoints, ARRAYSIZE(dirLightMeshProfilePoints));
  ProfileRotationParameters param;
  param.m_alignx     = 2;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = ROTATESMOOTH;
  param.m_rotateAxis = 2;
  LPD3DXMESH mesh = rotateProfile(getDevice(), prof, param, false);

  return optimizeMesh(mesh);
}

/*
void D3LightControlDirectional::OnD3D9ResetDevice( const D3DSURFACE_DESC &pBackBufferSurfaceDesc) {
  m_arcBall.SetWindow( pBackBufferSurfaceDesc.Width, pBackBufferSurfaceDesc.Height );
}

void D3LightControlDirectional::StaticOnD3D9LostDevice() {
  if(m_effect) {
    m_effect->OnLostDevice();
  }
}
*/

D3DXMATRIX D3LightControlDirectional::getWorldMatrix() const {
  const LIGHT light = getLightParam();
  return createWorldMatrix(-m_sphereRadius * light.Direction, &light.Direction);
}
