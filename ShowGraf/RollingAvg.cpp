#include "StdAfx.h"
#include "RollingAvg.h"

RollingAvg RollingAvg::s_default;

void RollingAvg::checkQueueSize(UINT queueSize) { // static
  if(!isValidQueueSize(queueSize)) {
    throwInvalidArgumentException(__TFUNCTION__
                                 ,_T("queueSize=%u. legal interval:[%u..%u]")
                                 ,queueSize
                                 ,getMinQueueSize()
                                 ,getMaxQueueSize()
                                 );
  }
}

void RollingAvg::setQueueSize(UINT queueSize) {
  checkQueueSize(queueSize);
  m_queueSize = queueSize;
}

bool operator==(const RollingAvg &r1, const RollingAvg &r2) {
  const bool c = r1.isEnabled() == r2.isEnabled();
  if(!c) return false;
  // enabled-values are the same
  if(!r1.isEnabled()) {
    return true; // if both are false, don't care about queuesize
  } else {
    return r1.getQueueSize() == r2.getQueueSize();
  }
}
