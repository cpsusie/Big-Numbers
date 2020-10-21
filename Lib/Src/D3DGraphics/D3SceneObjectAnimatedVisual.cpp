#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectWithMesh.h>
#include <D3DGraphics/D3SceneObjectVisualChild.h>
#include <D3DGraphics/D3SceneObjectAnimatedVisual.h>

class D3SceneObjectAnimatedVisualMeshFrame : public D3SceneObjectVisualChild<D3SceneObjectWithMesh> {
public:
  D3SceneObjectAnimatedVisualMeshFrame(D3SceneObjectAnimatedVisual *parent, LPD3DXMESH mesh, const String &name = EMPTYSTRING)
    : D3SceneObjectVisualChild(parent, mesh, name)
  {
    SAFEADDREF(mesh);
  }
};

D3SceneObjectAnimatedVisual::D3SceneObjectAnimatedVisual(D3Scene &scene, const MeshArray &meshArray, const String &name)
: D3SceneObjectVisual(   scene           , name)
, AbstractAnimator((UINT)meshArray.size(), name)
{
  init(meshArray);
}
D3SceneObjectAnimatedVisual::D3SceneObjectAnimatedVisual(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name)
: D3SceneObjectVisual(   parent          , name)
, AbstractAnimator((UINT)meshArray.size(), name)
{
  init(meshArray);
}

D3SceneObjectAnimatedVisual::~D3SceneObjectAnimatedVisual() {
  stopAnimation();
}

void D3SceneObjectAnimatedVisual::init(const MeshArray &meshArray) {
  m_fillMode          = D3DFILL_SOLID;
  m_shadeMode         = D3DSHADE_GOURAUD;
  m_lastRenderedIndex = -1;
  const UINT n = (UINT)meshArray.size();
  for(UINT i = 0; i < n; i++) {
    D3SceneObjectAnimatedVisualMeshFrame *child = new D3SceneObjectAnimatedVisualMeshFrame(this, meshArray[i]); TRACE_NEW(child);
    addChild(child);
  }
}

D3SceneObjectVisual *D3SceneObjectAnimatedVisual::getCurrentChild() const {
  const UINT cc = getChildCount();
  if(m_lastRenderedIndex < cc) {
    return getChild(m_lastRenderedIndex);
  }
  const UINT index = getCurrentIndex();
  return (index < cc) ? getChild(index) : nullptr;
}

void D3SceneObjectAnimatedVisual::handleTimeout(Timer &timer) {
  int oldIndex = getCurrentIndex();
  __super::handleTimeout(timer);
  const int newIndex = getCurrentIndex();
  getScene().setAnimationFrameIndex(oldIndex, newIndex);
}

void D3SceneObjectAnimatedVisual::draw() {
  m_lock.wait();
  const UINT cc    = getChildCount();
  const UINT index = getCurrentIndex();
  if(index < cc) {
    getChild(index)->draw();
    m_lastRenderedIndex = index;
  }
  m_lock.notify();
}

LPD3DXMESH D3SceneObjectAnimatedVisual::getMesh() const {
  const D3SceneObjectVisual *child = getCurrentChild();
  return child ? child->getMesh() : nullptr;
}
LPDIRECT3DVERTEXBUFFER D3SceneObjectAnimatedVisual::getVertexBuffer() const {
  const D3SceneObjectVisual *child = getCurrentChild();
  return child ? child->getVertexBuffer() : nullptr;
}
LPDIRECT3DINDEXBUFFER D3SceneObjectAnimatedVisual::getIndexBuffer() const {
  const D3SceneObjectVisual *child = getCurrentChild();
  return child ? child->getIndexBuffer() : nullptr;
}
bool D3SceneObjectAnimatedVisual::isNormalsVisible() const {
  const D3SceneObjectVisual *child = getCurrentChild();
  return child ? child->isNormalsVisible() : false;
}

void D3SceneObjectAnimatedVisual::setNormalsVisible(bool visible) {
  m_lock.wait();
  const UINT n = getChildCount();
  for(UINT i = 0; i < n; i++) {
    getChild(i)->setNormalsVisible(visible);
  }
  m_lock.notify();
}
