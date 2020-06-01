#pragma once

#include <MFCUtil/AnimationParameters.h>

class ParametricR2R3SurfaceParameters {
public:
  DoubleInterval      m_tInterval;
  DoubleInterval      m_sInterval;
  UINT                m_tStepCount;
  UINT                m_sStepCount;
  bool                m_doubleSided;
  AnimationParameters m_animation;

  ParametricR2R3SurfaceParameters();
  inline const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
  inline const DoubleInterval &getSInterval() const {
    return m_sInterval;
  }
  inline bool isAnimated() const {
    return m_animation.includeTime();
  }
};

