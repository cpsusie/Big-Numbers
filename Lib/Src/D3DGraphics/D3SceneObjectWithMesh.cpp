#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/MeshBuilder.h>

DECLARE_THISFILE;

// ----------------------------------- D3SceneObjectWithMesh --------------------------------------------

D3SceneObjectWithMesh::D3SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh)
: D3SceneObject(scene)
, m_fillMode(D3DFILL_SOLID)
, m_shadeMode(D3DSHADE_GOURAUD)
{
  m_mesh = mesh;
}

D3SceneObjectWithMesh::~D3SceneObjectWithMesh() {
  releaseMesh();
}

void D3SceneObjectWithMesh::createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf) {
  releaseMesh();
  m_mesh = getScene().allocateMesh(fvf, faceCount, vertexCount, D3DXMESH_SYSTEMMEM);
}

void D3SceneObjectWithMesh::releaseMesh() {
  SAFERELEASE(m_mesh);
}

void *D3SceneObjectWithMesh::lockVertexBuffer() {
  void *vertices;
  V(m_mesh->LockVertexBuffer(0, &vertices));
  return vertices;
}

void *D3SceneObjectWithMesh::lockIndexBuffer() {
  void *items;
  V(m_mesh->LockIndexBuffer(0, &items));
  return items;
}

void D3SceneObjectWithMesh::unlockVertexBuffer() {
  V(m_mesh->UnlockVertexBuffer());
}

void D3SceneObjectWithMesh::unlockIndexBuffer() {
  V(m_mesh->UnlockIndexBuffer());
}

D3DXCube3 D3SceneObjectWithMesh::getBoundingBox() const {
  return ::getBoundingBox(m_mesh) + getPos();
}

void D3SceneObjectWithMesh::draw() {
  setFillAndShadeMode();
  setSceneMaterial();
  setLightingEnable(true);
  drawSubset(0);
}

String D3SceneObjectWithMesh::toString() const {
  return format(_T("%s\nMesh:\n%s")
               ,getName().cstr()
               ,indentString(::toString(getMesh()),2).cstr());
}

