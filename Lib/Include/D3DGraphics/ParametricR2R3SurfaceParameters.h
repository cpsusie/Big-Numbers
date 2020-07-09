#pragma once

#include "D3SurfaceCommonParameters.h"

class ParametricR2R3SurfaceParameters : public D3SurfaceCommonParameters {
public:
  DoubleInterval      m_tInterval;
  DoubleInterval      m_sInterval;
  UINT                m_tStepCount;
  UINT                m_sStepCount;

  ParametricR2R3SurfaceParameters();
  inline const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
  inline const DoubleInterval &getSInterval() const {
    return m_sInterval;
  }
};

