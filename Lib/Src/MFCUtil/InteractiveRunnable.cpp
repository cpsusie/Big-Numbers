#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/InteractiveRunnable.h>

#pragma warning(disable : 4244)

USHORT InteractiveRunnable::getAvgSubProgressPercent() {
  const USHORT n = getJobCount();
  if(n == 0) return 0;
  UINT sum = 0;
  for(UINT i = 0; i < n; i++) {
    sum += getSubProgressPercent(i);
  }
  return sum / n;
}

UINT InteractiveRunnable::getEstimatedSecondsLeft() {
  const USHORT maxProgress = getMaxProgress();
  if(maxProgress == 0) {
    return 0;
  }
  const USHORT subPercent = (getSupportedFeatures() & IR_SUBPROGRESSBAR) ? getAvgSubProgressPercent() : 0;

  USHORT progress   = getProgress();
  const double q = (((double)progress + (double)subPercent/100)) / maxProgress;
  if(q == 0) {
    return maxProgress;
  } else if(q >= 1) {
    return 0;
  }
  const double secondsUsed = diff(m_jobStartTime, Timestamp(), TSECOND);

  return secondsUsed / q * (1-q);
}
