#pragma once

#include "QueueList.h"
#include "Date.h"
#include "Timer.h"
#include "FastSemaphore.h"

class _TimePctLogPoint { // For linear regression:(x,y) = (msec,pctDone)
public:
  double m_msec;     // distance to TimeEstimator.m_time0
  double m_pctDone;  // [0..100]
  inline _TimePctLogPoint(double msec=0, double pctDone=0) : m_msec(msec), m_pctDone(pctDone) {
  }
  inline String toString() const {
    return format(_T("(%7.0lf,%5.2lf)"), m_msec, m_pctDone);
  }
};

class ProgressProvider {
public:
  virtual double getProgress() const = 0; // must return [0..getMaxProgress()]
  virtual double getMaxProgress() const {
    return 100.0;
  }
  inline double getPercentDone() const {
    return PERCENT(getProgress(),getMaxProgress());
  }
  inline double getPermilleDone() const {
    return PERMILLE(getProgress(),getMaxProgress());
  }
};

class TimeEstimator : private TimeoutHandler {
private:
  const ProgressProvider      &m_progressProvider;
  Timer                        m_timer;
  QueueList<_TimePctLogPoint>  m_logQueue;
  UINT                         m_timeoutCount;
  Timestamp                    m_time0;
  double                       m_sumx, m_sumx2, m_sumy, m_sumxy;
  mutable bool                 m_needCalculateRegressionLine;
  mutable double               m_a, m_b;
  mutable FastSemaphore        m_gate;
  void logTimeAndPct();
  void appendLogPoint(double msec, double pctDone);
  void removeFirst();
  void initAllSums();
  void addTimePct(const _TimePctLogPoint &p);
  void subTimePct(const _TimePctLogPoint &p);
  void calculateRegressionLine() const;
  inline bool hasRegressionLine() const {
    return m_a != 0;
  }
  double getTimeEstimate() const;
public:
  TimeEstimator(const ProgressProvider &progressProvider);
  ~TimeEstimator();
  void handleTimeout(Timer &timer) {
    logTimeAndPct();
  }
  double getMilliSecondsRemaining() const;
};
