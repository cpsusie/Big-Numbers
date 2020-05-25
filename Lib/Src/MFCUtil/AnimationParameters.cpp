#include "pch.h"
#include <MFCUtil/AnimationParameters.h>

AnimationParameters &AnimationParameters::reset() {
  m_includeTime  = false;
  m_timeInterval = DoubleInterval(0, 10);
  m_frameCount   = 20;
  return *this;
}
