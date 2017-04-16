#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3CoordinateSystem.h>

D3CoordinateSystem::D3CoordinateSystem(D3Scene &scene) : D3SceneObject(scene, _T("CoordinateSystem")) {
  m_box   = new D3LineArray(scene, Vertex(-1,-1,-1), Vertex(1,1,1));
  m_xaxes = new D3LineArrow(scene, Vertex(-1, 0, 0), Vertex(1,0,0), D3D_RED  );
  m_yaxes = new D3LineArrow(scene, Vertex( 0,-1, 0), Vertex(0,1,0), D3D_GREEN);
  m_zaxes = new D3LineArrow(scene, Vertex( 0, 0,-1), Vertex(0,0,1), D3D_BLUE);
}

D3CoordinateSystem::~D3CoordinateSystem() {
  D3Scene &scene = m_box->getScene();
  delete m_box;
  delete m_xaxes;
  delete m_yaxes;
  delete m_zaxes;
}

void D3CoordinateSystem::draw() {
  m_box->draw();
  m_xaxes->draw();
  m_yaxes->draw();
  m_zaxes->draw();
}