#include "pch.h"
#include <D3DGraphics/ParametricR2R3SurfaceParameters.h>

ParametricR2R3SurfaceParameters::ParametricR2R3SurfaceParameters() {
  m_tInterval     = DoubleInterval(-10,10);
  m_sInterval     = DoubleInterval(-10,10);
  m_tStepCount    = 10;
  m_sStepCount    = 10;
  m_doubleSided   = true;
  m_animation.reset();
}
