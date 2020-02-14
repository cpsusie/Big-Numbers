#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>

class D3SceneGridObject : public D3SceneObjectLineArray {
private:
  UINT m_materialId;
  static CompactArray<Line3D> createLineArray(const D3DXCube3 &cube);
public:
  D3SceneGridObject(D3Scene &scene, const D3DXCube3 &cube) : D3SceneObjectLineArray(scene, createLineArray(cube)) {
    m_materialId = scene.addMaterial(MATERIAL::createMaterialWithColor(D3D_BLACK));
  }
  ~D3SceneGridObject() {
    getScene().removeMaterial(m_materialId);
  }
  void draw(const D3DXMATRIX &world) {
    getScene().setDevWorldMatrix(world).selectMaterial(m_materialId);
    __super::draw();
  }
};

#define GRIDLINECOUNT 10
CompactArray<Line3D> D3SceneGridObject::createLineArray(const D3DXCube3 &cube) {
  CompactArray<Line3D> result;
  const float xFrom = cube.getMinX(), xTo = cube.getMaxX();
  const float yFrom = cube.getMinY(), yTo = cube.getMaxY();
  const float z     = cube.getMinZ();
  D3DXVECTOR3 p1(xFrom, 0, z), p2(xTo, 0, z);
  for(int i = 0; i < GRIDLINECOUNT; i++) {
    const float t = (float)i / (GRIDLINECOUNT - 1), y = yFrom * (1.0f - t) + yTo * t;
    p1.y = p2.y = y;
    result.add(Line3D(p1, p2));
  }
  p1 = D3DXVECTOR3(0,yFrom, z), p2 = D3DXVECTOR3(0, yTo, z);
  for(int i = 0; i < GRIDLINECOUNT; i++) {
    const float t = (float)i / (GRIDLINECOUNT - 1), x = xFrom * (1.0f - t) + xTo * t;
    p1.x = p2.x = x;
    result.add(Line3D(p1, p2));
  }
  return result;
}

class D3CoordinateSystemFrameObject : public D3SceneObjectWithMesh {
private:
  D3SceneObjectCoordinateSystem &m_system;
  UINT                           m_materialId;
  D3SceneGridObject             *m_gridObject;
  static void       makeFace(MeshBuilder &mb, int v0, int v1, int v2, int v3);
  static LPD3DXMESH createFrameMesh(D3Scene &scene, const D3DXCube3 &cube);
public:
  D3CoordinateSystemFrameObject(D3SceneObjectCoordinateSystem *system);
  ~D3CoordinateSystemFrameObject();
  void draw(const D3DXMATRIX &world0);
};


D3SceneObjectCoordinateSystem::D3SceneObjectCoordinateSystem(D3Scene &scene, const D3DXCube3 *cube)
 : D3SceneObject(scene, _T("CoordinateSystem"))
 , m_axis(scene, Vertex(-1,0,0), Vertex(1,0,0))
 , m_pdus(scene.getRightHanded())
 , m_cube(cube ? *cube : D3DXCube3::getStdCube())
{
  m_pdus.resetPos().resetScale().setOrientation(D3DXVECTOR3(0, 1, 0), D3DXVECTOR3(0, 0, 1));
  m_axisMaterialId[0] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_RED  ));
  m_axisMaterialId[1] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_GREEN));
  m_axisMaterialId[2] = getScene().addMaterial(MATERIAL::createMaterialWithColor(D3D_BLUE ));
  m_frameObject = new D3CoordinateSystemFrameObject(this);
}

D3SceneObjectCoordinateSystem::~D3SceneObjectCoordinateSystem() {
  for(int i = 0; i < 3; i++) {
    m_scene.removeMaterial(m_axisMaterialId[i]);
  }
  SAFEDELETE(m_frameObject);
}

LPD3DXMESH D3SceneObjectCoordinateSystem::getMesh() const {
  return m_frameObject->getMesh();
}

void D3SceneObjectCoordinateSystem::draw() {
  const D3PosDirUpScale  &pdus = getPDUS();
  const D3DXMATRIX        worldX = pdus.getWorldMatrix();
  const D3DXMATRIX        worldY = worldX * createRotationMatrix(pdus.getUp(),  D3DX_PI / 2);
  const D3DXMATRIX        worldZ = worldX * createRotationMatrix(pdus.getDir(),-D3DX_PI / 2);
  // x-axis
  getScene().setDevWorldMatrix(worldX).selectMaterial(m_axisMaterialId[0]);
  m_axis.draw();
  // y-axis
  getScene().setDevWorldMatrix(worldY).selectMaterial(m_axisMaterialId[1]);
  m_axis.draw();
  // z-axis
  getScene().setDevWorldMatrix(worldZ).selectMaterial(m_axisMaterialId[2]);
  m_axis.draw();

  m_frameObject->draw(worldX);
}

void D3CoordinateSystemFrameObject::makeFace(MeshBuilder &mb, int v0, int v1, int v2, int v3) {
  Face &f = mb.addFace();
  const int nIndex = mb.addNormal(mb.calculateNormal(v0, v1, v2));
  f.addVertexNormalIndex(v0, nIndex);
  f.addVertexNormalIndex(v1, nIndex);
  f.addVertexNormalIndex(v2, nIndex);
  f.addVertexNormalIndex(v3, nIndex);
}

LPD3DXMESH D3CoordinateSystemFrameObject::createFrameMesh(D3Scene &scene, const D3DXCube3 &cube) {
  MeshBuilder mb;
  const D3DXVECTOR3 p1 = cube.getMin(), p2 = cube.getMax();
  const int c1 = mb.addVertex(p1.x, p1.y, p1.z);
  const int c2 = mb.addVertex(p1.x, p2.y, p1.z);
  const int c3 = mb.addVertex(p2.x, p2.y, p1.z);
  const int c4 = mb.addVertex(p2.x, p1.y, p1.z);

/*
  const int ltn = mb.addVertex(pmin.x, pmax.y, pmin.z); // left  top    near corner
  const int ltf = mb.addVertex(pmin.x, pmax.y, pmax.z); // left  top    far  corner
  const int rtn = mb.addVertex(pmax.x, pmax.y, pmin.z); // right top    near corner
  const int rtf = mb.addVertex(pmax.x, pmax.y, pmax.z); // right top    far  corner
*/
  makeFace(mb, c1, c2, c3, c4);
  return mb.createMesh(scene, true);
}

D3CoordinateSystemFrameObject::D3CoordinateSystemFrameObject(D3SceneObjectCoordinateSystem *system) 
: m_system(*system)
, D3SceneObjectWithMesh(system->getScene(), createFrameMesh(system->getScene(), system->getRange()))
{
  D3Scene &s = getScene();
  m_materialId = s.addMaterial(MATERIAL::createDefaultMaterial());
  MATERIAL m = s.getMaterial(m_materialId);
  s.setMaterial(m.setOpacity(0.3f));
  m_gridObject = new D3SceneGridObject(s, system->getRange()); TRACE_NEW(m_gridObject);
}

D3CoordinateSystemFrameObject::~D3CoordinateSystemFrameObject() {
  getScene().removeMaterial(m_materialId);
  SAFEDELETE(m_gridObject);
}

void D3CoordinateSystemFrameObject::draw(const D3DXMATRIX &world) {
  getScene().setDevWorldMatrix(world).setFillMode(D3DFILL_SOLID).selectMaterial(m_materialId);
  drawSubset(0);
  m_gridObject->draw(world);
}
