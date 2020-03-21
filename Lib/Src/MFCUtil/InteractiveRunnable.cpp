#include "pch.h"
#include <MFCUtil/InteractiveRunnable.h>

#pragma warning(disable : 4244)

void InteractiveRunnable::cleanup() {
  SAFEDELETE(m_timeEstimator);
}

const Timestamp &InteractiveRunnable::setStartTime() {
  cleanup();
  m_jobStartTime = Timestamp();
  if(getSupportedFeatures() & IR_SHOWTIMEESTIMATE) {
    m_timeEstimator = new TimeEstimator(*this); TRACE_NEW(m_timeEstimator);
  }
  return m_jobStartTime;
}

double InteractiveRunnable::getAvgSubProgressPercent() {
  const USHORT n = getJobCount();
  if(n == 0) {
    return 0;
  }
  UINT sum = 0;
  for(UINT i = 0; i < n; i++) {
    sum += getSubProgressPercent(i);
  }
  return sum / n;
}
