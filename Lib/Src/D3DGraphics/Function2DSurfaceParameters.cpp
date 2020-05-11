#include "pch.h"
#include <D3DGraphics/Function2DSurfaceParameters.h>

Function2DSurfaceParameters::Function2DSurfaceParameters() {
  m_xInterval     = DoubleInterval(-10,10);
  m_yInterval     = DoubleInterval(-10,10);
  m_pointCount    = 10;
  m_doubleSided   = true;
  m_includeTime   = false;
  m_timeInterval  = DoubleInterval(0,10);
  m_frameCount    = 20;
}

