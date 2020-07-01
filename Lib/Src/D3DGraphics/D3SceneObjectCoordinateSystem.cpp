#include "pch.h"
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3World.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneObjectLineArrow.h>
#include <D3DGraphics/D3SceneObjectLineArray.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>

static const D3DCOLOR axisColor[] = {
  D3D_RED
 ,D3D_GREEN
 ,D3D_BLUE
};

class D3SCoordSystemLineArrow : public D3SceneObjectLineArrow {
private:
  const int m_id;
  int       m_materialId;
public:
  D3SCoordSystemLineArrow(D3SceneObjectVisual *parent, int id);
  ~D3SCoordSystemLineArrow();
  int getMaterialId() const override {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override {
    return m_world = getParent()->getWorld();
  }
};

D3SCoordSystemLineArrow::D3SCoordSystemLineArrow(D3SceneObjectVisual *parent, int id)
: D3SceneObjectLineArrow(parent, D3DXORIGIN, createUnitVector(id), format(_T("%c axis"),'X'+id))
, m_id(     id)
{
  m_materialId = getScene().addMaterialWithColor(axisColor[id]);
}

D3SCoordSystemLineArrow::~D3SCoordSystemLineArrow() {
  getScene().removeMaterial(m_materialId);
}

class D3SceneGridObject : public D3SceneObjectLineArray {
private:
  UINT m_materialId;
  static CompactArray<Line3D> createLineArray(const D3Cube &cube);
public:
  D3SceneGridObject(D3SceneObjectVisual *parent, const D3Cube &cube)
    : D3SceneObjectLineArray(parent, createLineArray(cube), _T("GridObject"))
  {
    m_materialId = getScene().addMaterialWithColor(D3D_BLACK);
  }
  ~D3SceneGridObject() override {
    getScene().removeMaterial(m_materialId);
  }
  int getMaterialId() const override {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override {
    return m_world = getParent()->getWorld();
  }
};

#define GRIDLINECOUNT 10
CompactArray<Line3D> D3SceneGridObject::createLineArray(const D3Cube &cube) {
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

class VerticalAxisLineObject : public D3SceneObjectLineArray {
private:
  int m_materialId;
  static CompactArray<Line3D> createLineArray();

public:
  VerticalAxisLineObject(D3SceneObjectVisual *parent)
    : D3SceneObjectLineArray(parent, createLineArray(), _T("VerticalAxisLineObject"))
  {
    m_materialId = getScene().addMaterialWithColor(D3D_BLACK);
  }
  ~VerticalAxisLineObject() override {
    getScene().removeMaterial(m_materialId);
  }
  int getMaterialId() const override {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override {
    return m_world = getParent()->getWorld();
  }
};

CompactArray<Line3D> VerticalAxisLineObject::createLineArray() {
  CompactArray<Line3D> a;
  a.add(Line3D(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 2)));
  return a;
}

class VerticalAxisMeshObject : public D3SceneObjectWithMesh {
private:
  int m_materialId;
  static LPD3DXMESH createMesh(D3Device &device);
  D3DXVECTOR3 findCornerNearestCam() const;
public:
  VerticalAxisMeshObject(D3SceneObjectVisual *parent);
  ~VerticalAxisMeshObject() override;
  int getMaterialId() const override {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override;
};

VerticalAxisMeshObject::VerticalAxisMeshObject(D3SceneObjectVisual *parent)
: D3SceneObjectWithMesh(parent, createMesh(parent->getDevice()), _T("VerticalAxisMeshObject"))
{
  D3Scene &s = getScene();
  addChild(new VerticalAxisLineObject(this));
  m_materialId = s.addMaterial(D3Material::createDefaultMaterial());
  D3Material m   = s.getMaterial(m_materialId);
  s.setMaterial(m.setOpacity(0.3f));
}

VerticalAxisMeshObject::~VerticalAxisMeshObject() {
  getScene().removeMaterial(m_materialId);
}

LPD3DXMESH VerticalAxisMeshObject::createMesh(D3Device &device) { // static
  MeshBuilder mb;
  const int c1 = mb.addVertex(D3DXORIGIN);
  const int c2 = mb.addVertex(0.1f, 0, 0);
  const int c3 = mb.addVertex(0.1f, 0, 2);
  const int c4 = mb.addVertex(0   , 0, 2);
  return mb.addSquareFace(c1, c2, c3, c4).createMesh(device, true);
}

static const D3DXVECTOR3 bottomCorners[] = {
  D3DXVECTOR3( 1, 1,-1)
 ,D3DXVECTOR3(-1, 1,-1)
 ,D3DXVECTOR3(-1,-1,-1)
 ,D3DXVECTOR3( 1,-1,-1)
};

D3DXVECTOR3 VerticalAxisMeshObject::findCornerNearestCam() const {
  const D3DXVECTOR3 camPos   = getScene().getDevice().getCurrentCamera()->getPos();
  const D3DXMATRIX  &world   = getParent()->getWorld();
  float              minDist = -1;
  D3DXVECTOR3        result;
  for(int i = 0; i < 4; i++) {
    D3DXVECTOR3 c = world * bottomCorners[i];
    const float d = length(c - camPos);
    if((i == 0) || (d < minDist)) {
      result = c;
      minDist = d;
    }
  }
  return result;
}

D3DXMATRIX &VerticalAxisMeshObject::getWorld() {
  return m_world = D3World(getParent()->getWorld()).setPos(findCornerNearestCam());
}

class D3CoordinateSystemFrameObject : public D3SceneObjectWithMesh {
private:
  UINT m_materialId;
  static LPD3DXMESH createFrameMesh(D3Device &device, const D3Cube &cube);
public:
  D3CoordinateSystemFrameObject(D3SceneObjectVisual *parent, const D3Cube &cube);
  ~D3CoordinateSystemFrameObject();
  int getMaterialId() const {
    return m_materialId;
  }
  D3DXMATRIX &getWorld() override {
    return m_world = getParent()->getWorld();
  }
};

LPD3DXMESH D3CoordinateSystemFrameObject::createFrameMesh(D3Device &device, const D3Cube &cube) { // static
  MeshBuilder mb;
  const D3DXVECTOR3 p1 = cube.getMin(), p2 = cube.getMax();
  const int c1 = mb.addVertex(p1.x, p1.y, p1.z);
  const int c2 = mb.addVertex(p1.x, p2.y, p1.z);
  const int c3 = mb.addVertex(p2.x, p2.y, p1.z);
  const int c4 = mb.addVertex(p2.x, p1.y, p1.z);
  return mb.addSquareFace(c1, c2, c3, c4).createMesh(device, true);
}

D3CoordinateSystemFrameObject::D3CoordinateSystemFrameObject(D3SceneObjectVisual *parent, const D3Cube &cube)
: D3SceneObjectWithMesh(parent, createFrameMesh(parent->getDevice(), cube), _T("FrameObject"))
{
  D3Scene &s = getScene();
  m_materialId = s.addMaterial(D3Material::createDefaultMaterial());
  D3Material m = s.getMaterial(m_materialId);
  s.setMaterial(m.setOpacity(0.3f));
  addChild(new D3SceneGridObject(     this, cube));
  addChild(new VerticalAxisMeshObject(this));
}

D3CoordinateSystemFrameObject::~D3CoordinateSystemFrameObject() {
  getScene().removeMaterial(m_materialId);
}

D3SceneObjectCoordinateSystem::D3SceneObjectCoordinateSystem(D3Scene &scene, const D3Cube *cube)
  : D3SceneObjectVisual(scene, _T("CoordinateSystem"))
  , m_cube(cube ? *cube : D3Cube::getStdCube())
{
  resetWorld();
  for(int i = 0; i < 3; i++) {
    addChild(new D3SCoordSystemLineArrow(this, i));
  }
  addChild(new D3CoordinateSystemFrameObject(this, m_cube));
}

D3DXMATRIX &D3SceneObjectCoordinateSystem::getWorld() {
  return m_world = D3World(m_world).resetPos();
}
