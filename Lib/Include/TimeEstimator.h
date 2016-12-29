#pragma once

#include <QueueList.h>
#include <Date.h>
#include <Timer.h>

class _TimePctLogPoint { // For linear regression:(x,y) = (msec,pctDone)
public:
  double    m_msec; // distance to TimeEstimator.m_time0
  double    m_pctDone;
  _TimePctLogPoint(double msec=0, double pctDone=0) : m_msec(msec), m_pctDone(pctDone) {
  }
  String toString() const {
    return format(_T("(%7.0lf,%5.2lf)"), m_msec, m_pctDone);
  }
};

class ProgressContainer {
public:
  virtual double getPercentDone() const = 0; // must return [0..100]
};

class TimeEstimator : private QueueList<_TimePctLogPoint>, private TimeoutHandler {
private:
  const ProgressContainer &m_progressContainer;
  Timestamp                m_time0;
  double                   m_sumx,m_sumx2,m_sumy,m_sumxy;
  double                   m_a, m_b;
  Timer                    m_timer;
  UINT                     m_timeoutCount;
  mutable Semaphore        m_gate;
  void logTimeAndPct();
  void appendLogPoint(double msec, double pctDone);
  void removeFirst();
  void initAllSums();
  void addTimePct(const _TimePctLogPoint &p);
  void subTimePct(const _TimePctLogPoint &p);
  void calculateRegressionLine();
  inline bool hasRegressionLine() const {
    return m_a != 0;
  }
  double getTimeEstimate() const;
public:
  TimeEstimator(const ProgressContainer &progressContainer);
  ~TimeEstimator() {
    m_timer.stopTimer();
  }
  void handleTimeout(Timer &timer) {
    logTimeAndPct();
  }
  double getMilliSecondsRemaining() const;
};
