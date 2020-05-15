#include "pch.h"
#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>

// ----------------------------------- D3SceneObjectWithMesh --------------------------------------------

D3SceneObjectWithMesh::D3SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh, const String &name)
: D3SceneObjectVisual(scene, name)
, m_fillMode( D3DFILL_SOLID)
, m_shadeMode(D3DSHADE_GOURAUD)
{
  m_mesh = mesh;
}

D3SceneObjectWithMesh::D3SceneObjectWithMesh(D3SceneObjectVisual *parent, LPD3DXMESH mesh, const String &name)
: D3SceneObjectVisual(parent, name)
, m_fillMode( D3DFILL_SOLID)
, m_shadeMode(D3DSHADE_GOURAUD)
{
  m_mesh = mesh;
}

D3SceneObjectWithMesh::~D3SceneObjectWithMesh() {
  releaseMesh();
}

void D3SceneObjectWithMesh::createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf) {
  releaseMesh();
  m_mesh = getDevice().allocateMesh(fvf, faceCount, vertexCount, D3DXMESH_SYSTEMMEM);
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

D3Cube D3SceneObjectWithMesh::getBoundingBox() const {
  return ::getBoundingBox(m_mesh) + D3World(*this).getPos();
}

void D3SceneObjectWithMesh::draw() {
  if(hasMesh()) {
    D3Device &device = setDeviceMaterialIfExist();
    device.setWorldMatrix(getWorld())
          .setFillMode(getFillMode())
          .setShadeMode(getShadeMode())
          .setLightingEnable(getLightingEnable());
    drawSubset(0);
  }
  __super::draw();
}

String D3SceneObjectWithMesh::toString() const {
  return format(_T("%s\nMesh:\n%s")
               ,getName().cstr()
               ,indentString(::toString(getMesh()),2).cstr());
}

static String getMeshString(LPD3DXMESH mesh) {
  return ::toString(mesh, FORMAT_BUFFERDESC | FORMAT_VERTEXBUFFER)
    + "\n"
    + ::toString(mesh, FORMAT_BUFFERDESC | FORMAT_INDEXBUFFER);
}

String D3SceneObjectWithMesh::getInfoString() const {
  String result = __super::getInfoString();
  if(hasMesh()) {
    if(result.length()) result += "\n";
    result += getMeshString(getMesh());
  }
  return result.trimRight();
}
