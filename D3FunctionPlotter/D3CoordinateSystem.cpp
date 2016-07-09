#include "stdafx.h"
#include "D3CoordinateSystem.h"

D3CoordinateSystem::D3CoordinateSystem(D3Scene &scene) {
  m_box   = new D3LineArray(scene, createVertex(-1,-1,-1), createVertex(1,1,1));
  m_xaxes = new D3LineArrow(scene, createVertex(-1, 0, 0), createVertex(1,0,0), D3D_RED  );
  m_yaxes = new D3LineArrow(scene, createVertex( 0,-1, 0), createVertex(0,1,0), D3DCOLOR_XRGB(0  ,200,  0));
  m_zaxes = new D3LineArrow(scene, createVertex( 0, 0,-1), createVertex(0,0,1), D3D_BLUE);
  scene.addSceneObject(m_box);
  scene.addSceneObject(m_xaxes);
  scene.addSceneObject(m_yaxes);
  scene.addSceneObject(m_zaxes);
}

D3CoordinateSystem::~D3CoordinateSystem() {
  D3Scene &scene = m_box->getScene();
  scene.removeSceneObject(m_box);
  scene.removeSceneObject(m_xaxes);
  scene.removeSceneObject(m_yaxes);
  scene.removeSceneObject(m_zaxes);
  delete m_box;
  delete m_xaxes;
  delete m_yaxes;
  delete m_zaxes;
}

void D3CoordinateSystem::setVisible(bool visible) {
  m_box->setVisible(  visible);
  m_xaxes->setVisible(visible);
  m_yaxes->setVisible(visible);
  m_zaxes->setVisible(visible);
}

