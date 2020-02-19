#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>

D3SceneObjectAnimatedMesh::D3SceneObjectAnimatedMesh(D3Scene &scene, const MeshArray &meshArray)
: D3SceneObjectVisual(scene)
, m_meshArray(meshArray)
, m_frameCount((UINT)meshArray.size())
, m_fillMode(D3DFILL_SOLID)
, m_shadeMode(D3DSHADE_GOURAUD)
, m_timer(1, format(_T("Timer for animated mesh %s"), getName().cstr()))
, m_sleepTime(50)
, m_animationType(ANIMATE_FORWARD)
, m_forward(true)
, m_running(false)
, m_nextMeshIndex(0)
, m_lastRenderedIndex(-1)
{
}

D3SceneObjectAnimatedMesh::~D3SceneObjectAnimatedMesh() {
  stopAnimation();
}

void D3SceneObjectAnimatedMesh::startAnimation(AnimationType type) {
  if(isRunning()) {
    return;
  }
  m_animationType = type;
  m_timer.startTimer(getSleepTime(), *this, true);
  m_running = true;
}

void D3SceneObjectAnimatedMesh::stopAnimation() {
  if(isRunning()) {
    m_timer.stopTimer();
    m_running = false;
  }
}

void D3SceneObjectAnimatedMesh::scaleSpeed(float factor) {
  if((factor == 0) || (factor == 1)) {
    return;
  }
  m_sleepTime /= factor;
  if(m_sleepTime < 1) {
    m_sleepTime = 1;
  }
  if(isRunning()) {
    m_timer.setTimeout(getSleepTime(), true);
  }
}

void D3SceneObjectAnimatedMesh::handleTimeout(Timer &t) {
  getScene().setAnimationFrameIndex(m_lastRenderedIndex, m_nextMeshIndex);
  nextIndex();
  if(getAnimationType() == ANIMATE_ALTERNATING) {
    m_timer.setTimeout(getSleepTime(), true);
  }
}

int D3SceneObjectAnimatedMesh::getSleepTime() const {
  switch(getAnimationType()) {
  case ANIMATE_FORWARD    :
  case ANIMATE_BACKWARD   :
    return (int)m_sleepTime;
  case ANIMATE_ALTERNATING:
    { const double m = (double)m_frameCount/2;
      return (int)(m_sleepTime * (1.0 + sqr(fabs((double)m_nextMeshIndex - m) / m*2)));
    }
  }
  return (int)m_sleepTime;
}

//  invariant:m_nextMeshIndex = [0..m_frameCount-1]
void D3SceneObjectAnimatedMesh::nextIndex() {
  switch(getAnimationType()) {
  case ANIMATE_FORWARD    :
    m_nextMeshIndex = (m_nextMeshIndex + 1) % m_frameCount;
    break;
  case ANIMATE_BACKWARD   :
    if(--m_nextMeshIndex < 0) {
      m_nextMeshIndex = m_frameCount-1;
    }
    break;
  case ANIMATE_ALTERNATING:
    if(m_forward) {
      if(++m_nextMeshIndex == m_frameCount-1) {
        m_forward = false;
      }
    } else {
      if(--m_nextMeshIndex == 0) {
        m_forward = true;
      }
    }
    break;
  }
}

void D3SceneObjectAnimatedMesh::draw() {
  getDevice().setWorldMatrix(getWorld())
             .setFillMode(getFillMode())
             .setShadeMode(getShadeMode())
             .setMaterial(getMaterial());
  V(m_meshArray[m_lastRenderedIndex = m_nextMeshIndex]->DrawSubset(0));
}

LPD3DXMESH D3SceneObjectAnimatedMesh::getMesh() const {
  if(m_lastRenderedIndex < 0) {
    return NULL;
  }
  return m_meshArray[m_lastRenderedIndex];
}
