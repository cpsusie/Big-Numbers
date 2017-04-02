#include "pch.h"
#include <D3DGraphics/D3Scene.h>

DECLARE_THISFILE;

class MeshAnimationThread : public Thread {
private:
  D3AnimatedSurface &m_surface;
  int               &m_nextMeshIndex;
  const UINT         m_frameCount;
  Semaphore          m_timeout;
  double             m_sleepTime;
  bool               m_killed;
  bool               m_running;
  bool               m_forward;
  AnimationType      m_type;
  void nextIndex();
  int  getSleepTime() const;
public:
  MeshAnimationThread(D3AnimatedSurface &surface) 
    : m_surface(surface)
    , m_nextMeshIndex(m_surface.m_nextMeshIndex)
    , m_frameCount((UINT)m_surface.m_meshArray.size())
    , m_timeout(0)
  {
    m_killed    = false;
    m_running   = false;
    m_type      = ANIMATE_FORWARD;
    m_forward   = true;
    m_sleepTime = 50;
    resume();
  }
  void go(AnimationType type);
  void stop();
  void kill();
  void scaleSpeed(double factor);
  bool isRunning() const {
    return m_running;
  }
  AnimationType getAnimationType() const {
    return m_type;
  }
  unsigned int run();
};

void MeshAnimationThread::go(AnimationType type) {
  if(isRunning()) {
    return;
  }
  m_type = type;
  switch(type) {
  case ANIMATE_FORWARD    :
    m_forward = true;
    break;
  case ANIMATE_BACKWARD   :
    m_forward = false;
    break;
  case ANIMATE_ALTERNATING:
    break;
  }

  m_running = true;
  m_timeout.signal();
}

void MeshAnimationThread::stop() {
  if(!isRunning()) {
    return;
  }
  m_running = false;
}

void MeshAnimationThread::kill() {
  m_killed = true;
  go(ANIMATE_FORWARD);
  for(int i = 0; stillActive() && (i < 100); i++) {
    Sleep(20);
  }
  if(stillActive()) {
    AfxMessageBox(_T("Cannot not stop animationThread"), MB_ICONSTOP);
  }
}

void MeshAnimationThread::scaleSpeed(double factor) {
  if(factor == 0) {
    return;
  }
  m_sleepTime /= factor;
  if(m_sleepTime < 1) {
    m_sleepTime = 1;
  }
}

unsigned int MeshAnimationThread::run() {
  for(;;) {
    m_timeout.wait(m_running ? getSleepTime() : INFINITE);
    if(m_killed) {
      break;
    }
    m_surface.getScene().setAnimationFrameIndex(m_surface.m_lastRenderedIndex, m_nextMeshIndex);
    nextIndex();
  }
  return 0;
}

int MeshAnimationThread::getSleepTime() const {
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

void MeshAnimationThread::nextIndex() { //  invariant:m_nextMeshIndex = [0..m_frameCount-1]
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

D3AnimatedSurface::D3AnimatedSurface(D3Scene &scene, const MeshArray &meshArray) : D3SceneObject(scene) {
  m_meshArray         = meshArray;
  m_nextMeshIndex     = 0;
  m_lastRenderedIndex = -1;
  m_animator          = new MeshAnimationThread(*this);
}

D3AnimatedSurface::~D3AnimatedSurface() {
  m_animator->kill();
  delete m_animator;
}

void D3AnimatedSurface::startAnimation(AnimationType type) {
  m_animator->go(type);
}

void D3AnimatedSurface::stopAnimation() {
  m_animator->stop();
}

bool D3AnimatedSurface::isRunning() const {
  return m_animator->isRunning();
}

AnimationType D3AnimatedSurface::getAnimationType() const {
  return m_animator->getAnimationType();
}

void D3AnimatedSurface::scaleSpeed(double factor) { // sleepTime /= factor
  m_animator->scaleSpeed(factor);
}

void D3AnimatedSurface::draw() {
  D3AnimatedSurface::prepareDraw();
  V(m_meshArray[m_lastRenderedIndex = m_nextMeshIndex]->DrawSubset(0));
}

LPD3DXMESH D3AnimatedSurface::getMesh() const {
  if(m_lastRenderedIndex < 0) {
    return NULL;
  }
  return m_meshArray[m_lastRenderedIndex];
}

