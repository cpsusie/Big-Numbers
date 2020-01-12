#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>

D3SceneObjectCoordinateSystem::D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube) : D3SceneObject(scene, _T("CoordinateSystem")) {
  init();
  setCube(cube ? *cube : D3DXCube3::getStdCube());
}

D3SceneObjectCoordinateSystem::D3SceneObjectCoordinateSystem(const D3SceneObjectCoordinateSystem &src) : D3SceneObject(getScene(), getName()) {
  init();
  setCube(src.getCube());
}

D3SceneObjectCoordinateSystem &D3SceneObjectCoordinateSystem::operator=(const D3SceneObjectCoordinateSystem &src) {
  setCube(src.getCube());
  return *this;
}

D3SceneObjectCoordinateSystem::~D3SceneObjectCoordinateSystem() {
  cleanUp();
}

void D3SceneObjectCoordinateSystem::init() {
  m_box   = NULL;
  m_xaxis = m_yaxis = m_zaxis = NULL;
}

void D3SceneObjectCoordinateSystem::cleanUp() {
  SAFEDELETE(m_box);
  SAFEDELETE(m_xaxis);
  SAFEDELETE(m_yaxis);
  SAFEDELETE(m_zaxis);
}

void D3SceneObjectCoordinateSystem::setCube(const D3DXCube3 &cube) {
  setCube(cube.getMinX(), cube.getMinY(), cube.getMinZ()
         ,cube.getMaxX(), cube.getMaxY(), cube.getMaxZ());
}

void D3SceneObjectCoordinateSystem::setCube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
  D3Scene &scene = getScene();
  cleanUp();
  m_cube  = D3DXCube3(minX,minY,minZ, maxX,maxY,maxZ);
  m_box   = new D3SceneObjectWireFrameBox(scene, Vertex(minX,minY,minZ), Vertex(maxX,maxY,maxZ)); TRACE_NEW(m_box);
  const float mmx = minMax(0.f,minX,maxX), mmy = minMax(0.f,minY,maxY), mmz = minMax(0.f,minZ,maxZ);
  m_xaxis = new D3SceneObjectLineArrow(scene, Vertex(minX,mmy , mmz ), Vertex(maxX,mmy ,mmz ), D3D_RED  ); TRACE_NEW(m_xaxis);
  m_yaxis = new D3SceneObjectLineArrow(scene, Vertex(mmx ,minY, mmz ), Vertex(mmx ,maxY,mmz ), D3D_GREEN); TRACE_NEW(m_yaxis);
  m_zaxis = new D3SceneObjectLineArrow(scene, Vertex(mmx ,mmy , minZ), Vertex(mmx ,mmy ,maxZ), D3D_BLUE ); TRACE_NEW(m_zaxis);
}

void D3SceneObjectCoordinateSystem::draw() {
  m_box->draw();
  m_xaxis->draw();
  m_yaxis->draw();
  m_zaxis->draw();
}
