#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>

D3SceneObjectAnimatedMesh::D3SceneObjectAnimatedMesh(D3Scene &scene, const MeshArray &meshArray, const String &name)
: D3SceneObjectVisual(scene,name)
, AbstractAnimator((UINT)meshArray.size(), name)
, m_meshArray(meshArray)
{
  init();
}

D3SceneObjectAnimatedMesh::D3SceneObjectAnimatedMesh(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name)
: D3SceneObjectVisual(parent,name)
, AbstractAnimator((UINT)meshArray.size(), name)
, m_meshArray(        meshArray)
{
  init();
}

void D3SceneObjectAnimatedMesh::init() {
  m_fillMode          = D3DFILL_SOLID;
  m_shadeMode         = D3DSHADE_GOURAUD;
  m_lastRenderedIndex = -1;
}

D3SceneObjectAnimatedMesh::~D3SceneObjectAnimatedMesh() {
  stopAnimation();
}

void D3SceneObjectAnimatedMesh::handleTimeout(Timer &t) {
  int oldIndex = getCurrentIndex();
  __super::handleTimeout(t);
  const int newIndex = getCurrentIndex();
  getScene().setAnimationFrameIndex(oldIndex, newIndex);
}

void D3SceneObjectAnimatedMesh::draw() {
  const UINT index = getCurrentIndex();
  if(index >= m_meshArray.size()) {
    return;
  }
  D3Device &device = setDeviceMaterialIfExist();
  setDeviceTextureIfExist();
  device.setWorldMatrix(getWorld())
        .setFillMode(getFillMode())
        .setShadeMode(getShadeMode())
        .setLightingEnable(getLightingEnable());
  V(m_meshArray[m_lastRenderedIndex = index]->DrawSubset(0));
  __super::draw();
}

LPD3DXMESH D3SceneObjectAnimatedMesh::getMesh() const {
  if(m_lastRenderedIndex < m_meshArray.size()) {
    return m_meshArray[m_lastRenderedIndex];
  }
  const UINT index = getCurrentIndex();
  if(index < m_meshArray.size()) {
    return m_meshArray[index];
  }
  return NULL;
}
