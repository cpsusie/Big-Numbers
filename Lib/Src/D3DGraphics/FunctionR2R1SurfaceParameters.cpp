#include "pch.h"
#include <D3DGraphics/FunctionR2R1SurfaceParameters.h>

FunctionR2R1SurfaceParameters::FunctionR2R1SurfaceParameters() {
  m_xInterval     = DoubleInterval(-10,10);
  m_yInterval     = DoubleInterval(-10,10);
  m_pointCount    = 10;
  m_doubleSided   = true;
  m_animation.reset();
}

