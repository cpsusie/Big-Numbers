#include "pch.h"
#include <D3DGraphics/ParametricR1R3SurfaceParameters.h>

ParametricR1R3SurfaceParameters::ParametricR1R3SurfaceParameters() {
  m_tInterval     = DoubleInterval(-10,10);
  m_tStepCount    = 10;
}
