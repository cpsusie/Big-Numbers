#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>

D3SceneObjectCoordinateSystem::D3SceneObjectCoordinateSystem(D3Scene &scene)
  : D3SceneObject(scene, _T("CoordinateSystem"))
  , m_axis(scene, Vertex(-1,0,0), Vertex(1,0,0))
  , m_origin(scene.getRightHanded())
{
  m_axisMaterialId[0] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_RED  ));
  m_axisMaterialId[1] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_GREEN));
  m_axisMaterialId[2] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_BLUE ));
}

D3SceneObjectCoordinateSystem::~D3SceneObjectCoordinateSystem() {
  for(int i = 0; i < 3; i++) {
    m_scene.removeMaterial(m_axisMaterialId[i]);
  }
}

void D3SceneObjectCoordinateSystem::draw() {
  LPDIRECT3DDEVICE device = getScene().getDevice();
  D3PosDirUpScale pdus(getScene().getRightHanded());
  pdus.resetPos().resetScale().setOrientation(D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(0, 0, 1));
  D3DXMATRIX world0 = pdus.getWorldMatrix(), rot;

  // x-axis
  getScene().selectMaterial(m_axisMaterialId[0]);
  V(device->SetTransform(D3DTS_WORLD, &world0));
  m_axis.draw();

  // y-axis
  D3DXMatrixRotationZ(&rot, D3DX_PI / 2);
  rot = world0 * rot;
  getScene().selectMaterial(m_axisMaterialId[1]);
  V(device->SetTransform(D3DTS_WORLD, &rot));
  m_axis.draw();

  // z-axis
  D3DXMatrixRotationY(&rot, -D3DX_PI / 2);
  rot = world0 * rot;
  getScene().selectMaterial(m_axisMaterialId[2]);
  V(device->SetTransform(D3DTS_WORLD, &rot));
  m_axis.draw();
}
