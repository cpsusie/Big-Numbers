#pragma once

#include "D3SurfaceCommonParameters.h"

class FunctionR2R1SurfaceParameters : public D3SurfaceCommonParameters {
public:
  DoubleInterval            m_xInterval;
  DoubleInterval            m_yInterval;
  UINT                      m_pointCount;
  FunctionR2R1SurfaceParameters();
  const DoubleInterval &getXInterval() const {
    return m_xInterval;
  }
  const DoubleInterval &getYInterval() const {
    return m_yInterval;
  }
};
