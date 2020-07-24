#pragma once

#include "D3SurfaceCommonParameters.h"

class ParametricR1R3SurfaceParameters : public D3SurfaceCommonParameters {
public:
  DoubleInterval      m_tInterval;
  UINT                m_tStepCount;

  ParametricR1R3SurfaceParameters();
  inline const DoubleInterval &getTInterval() const {
    return m_tInterval;
  }
};

