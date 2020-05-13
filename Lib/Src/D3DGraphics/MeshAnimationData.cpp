#include "pch.h"
#include <D3DGraphics/MeshAnimationData.h>

MeshAnimationData &MeshAnimationData::reset() {
  m_includeTime  = false;
  m_timeInterval = DoubleInterval(0, 10);
  m_frameCount   = 20;
  return *this;
}
