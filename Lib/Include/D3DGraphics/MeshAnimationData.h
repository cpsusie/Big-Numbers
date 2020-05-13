#pragma once

#include <NumberInterval.h>

class MeshAnimationData {
public:
  bool           m_includeTime;
  DoubleInterval m_timeInterval;
  // Number of frames (meshes) that will be generated
  UINT           m_frameCount;
  // If true, an animation will be generated
  MeshAnimationData() {
    reset();
  }
  inline const DoubleInterval &getTimeInterval() const {
    return m_timeInterval;
  }
  inline UINT getFrameCount() const {
    return m_frameCount;
  }
  inline bool includeTime() const {
    return m_includeTime;
  }
  MeshAnimationData &reset();
};
