#pragma once

#include <Timer.h>
#include "D3SceneObjectVisual.h"
#include "MeshArray.h"

typedef enum {
  ANIMATE_FORWARD
 ,ANIMATE_BACKWARD
 ,ANIMATE_ALTERNATING
} AnimationType;

class D3SceneObjectAnimatedMesh : public D3SceneObjectVisual, public TimeoutHandler {
private:
  MeshArray     m_meshArray;
  const UINT    m_frameCount;
  Timer         m_timer;
  D3DFILLMODE   m_fillMode;
  D3DSHADEMODE  m_shadeMode;
  float         m_sleepTime;
  AnimationType m_animationType;
  bool          m_forward;
  bool          m_running;
  int           m_nextMeshIndex, m_lastRenderedIndex;
  void nextIndex();
  int  getSleepTime() const;
  void init();
public:
  D3SceneObjectAnimatedMesh(D3Scene             &scene , const MeshArray &meshArray, const String &name = _T("Animated Mesh"));
  D3SceneObjectAnimatedMesh(D3SceneObjectVisual *parent, const MeshArray &meshArray, const String &name = _T("Animated Mesh"));
  ~D3SceneObjectAnimatedMesh();
  void startAnimation(AnimationType type = ANIMATE_FORWARD);
  void stopAnimation();
  void handleTimeout(Timer &timer);
  inline bool isRunning() const {
    return m_running;
  }
  inline AnimationType getAnimationType() const {
    return m_animationType;
  }
  // sleepTime /= factor
  void scaleSpeed(float factor);
  inline float getFramePerSec() const {
    return m_running ? 1000.0f / m_sleepTime : 0;
  }
  LPD3DXMESH getMesh() const;
  void draw();
  bool hasFillMode() const {
    return true;
  }
  void setFillMode(D3DFILLMODE fillMode) {
    m_fillMode = fillMode;
  }
  D3DFILLMODE getFillMode() const {
    return m_fillMode;
  }
  bool hasShadeMode() const {
    return true;
  }
  void setShadeMode(D3DSHADEMODE shadeMode) {
    m_shadeMode = shadeMode;
  }
  D3DSHADEMODE getShadeMode() const {
    return m_shadeMode;
  }
  SceneObjectType getType() const {
    return SOTYPE_ANIMATEDOBJECT;
  }
};
