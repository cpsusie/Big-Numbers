#include "stdafx.h"
#include <MyUtil.h>
#include <MFCUtil/InteractiveRunnable.h>

#pragma warning(disable : 4244)

unsigned int InteractiveRunnable::getEstimatedSecondsLeft() {
  const unsigned short maxProgress = getMaxProgress();
  if(maxProgress == 0) {
    return 0;
  }
  const unsigned short subPercent = (getSupportedFeatures() & IR_SUBPROGRESSBAR) ? getSubProgressPercent() : 0;

  unsigned short progress   = getProgress();
  const double q = (((double)progress + (double)subPercent/100)) / maxProgress;
  if(q == 0) {
    return maxProgress;
  } else if(q >= 1) {
    return 0;
  }
  const double secondsUsed = diff(m_jobStartTime, Timestamp(), TSECOND);

  return secondsUsed / q * (1-q);
}

