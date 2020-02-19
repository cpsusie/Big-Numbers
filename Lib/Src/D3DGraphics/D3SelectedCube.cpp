#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3SelectedCube.h>

D3SelectedCube::D3SelectedCube(D3Scene &scene, const D3DXCube3 &cube) : D3SceneObjectSolidBox(scene, cube) {
  setName(_T("SelectedCube"));
}

void D3SelectedCube::setCube(const D3DXCube3 &cube) {
}
// TODO
/*
void D3SelectedCube::setCube(float minX, float minY, float minZ, float maxX, float maxY, float maxZ) {
  D3Scene &scene = getScene();
  m_cube  = D3DXCube3(minX,minY,minZ, maxX,maxY,maxZ);
  m_box   = new D3WireFrameBox(scene, Vertex(minX,minY,minZ), Vertex(maxX,maxY,maxZ)); TRACE_NEW(m_box);
  const float mmx = minMax(0.f,minX,maxX), mmy = minMax(0.f,minY,maxY), mmz = minMax(0.f,minZ,maxZ);
  m_xaxis = new D3LineArrow(scene, Vertex(minX,mmy , mmz ), Vertex(maxX,mmy ,mmz ), D3D_RED  ); TRACE_NEW(m_xaxis);
  m_yaxis = new D3LineArrow(scene, Vertex(mmx ,minY, mmz ), Vertex(mmx ,maxY,mmz ), D3D_GREEN); TRACE_NEW(m_yaxis);
  m_zaxis = new D3LineArrow(scene, Vertex(mmx ,mmy , minZ), Vertex(mmx ,mmy ,maxZ), D3D_BLUE ); TRACE_NEW(m_zaxis);
}
*/
