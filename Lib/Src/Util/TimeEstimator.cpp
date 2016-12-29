#include "pch.h"
#include <TimeEstimator.h>

TimeEstimator::TimeEstimator(const ProgressContainer &progressContainer) 
: m_progressContainer(progressContainer)
, m_timer(1)
, m_timeoutCount(0)
{
  initAllSums();
  logTimeAndPct();
  m_timer.startTimer(200, *this, true);
}

void TimeEstimator::logTimeAndPct() {
  m_gate.wait();

  const Timestamp now;
  const double    pctDone = m_progressContainer.getPercentDone();
  const size_t    n       = size();
  if(n == 0) {
    m_time0 = now;
    appendLogPoint(0, pctDone);
  } else {
    if(n >= 10) {
      removeFirst();
    }
    appendLogPoint(diff(m_time0, now, TMILLISECOND), pctDone);
    calculateRegressionLine();
  }
  m_gate.signal();
  switch(++m_timeoutCount) {
  case 10  : m_timer.setTimeout(1000, true); break;
  case 100 : m_timer.setTimeout(2000, true); break;
  case 1000: m_timer.setTimeout(4000, true); break;
  }
}

void TimeEstimator::appendLogPoint(double msec, double pctDone) {
  const _TimePctLogPoint p(msec, pctDone);
  put(p);
  addTimePct(p);
}

void TimeEstimator::removeFirst() { // assume queue not empty
  const _TimePctLogPoint first = get();
  if(first.m_msec < 800000) {
    subTimePct(first);
  } else { // avoid truncation errors if mseconds becomre to large
    m_time0.add(TMILLISECOND, (int)first.m_msec);
    initAllSums();
    for (Iterator<_TimePctLogPoint> it = getIterator(); it.hasNext();) {
      _TimePctLogPoint &p = it.next();
      p.m_msec -= first.m_msec;
      addTimePct(p);
    }
  }
}

void TimeEstimator::initAllSums() {
  m_sumx = m_sumx2 = m_sumy = m_sumxy = 0;
  m_a = m_b = 0;
}

void TimeEstimator::addTimePct(const _TimePctLogPoint &p) {
  m_sumx  += p.m_msec;
  m_sumx2 += sqr(p.m_msec);
  m_sumy  += p.m_pctDone;
  m_sumxy += p.m_msec*p.m_pctDone;
}

void TimeEstimator::subTimePct(const _TimePctLogPoint &p) {
  m_sumx  -= p.m_msec;
  m_sumx2 -= sqr(p.m_msec);
  m_sumy  -= p.m_pctDone;
  m_sumxy -= p.m_msec*p.m_pctDone;
}

void TimeEstimator::calculateRegressionLine() { // assume queue.size() >= 2
  const double n = (double)size();
  m_a = (m_sumxy - m_sumx * m_sumy/n) / (m_sumx2 - sqr(m_sumx)/n);
  if(hasRegressionLine()) {
    m_b = (m_sumy - m_a * m_sumx) / n;
/*
    debugLog(_T("Count:%2u t0:%s. %-90s a:%.3le, b:%.3le\n")
            ,m_timeoutCount
            ,m_time0.getTime().toString(hhmmss).cstr()
            ,toString().cstr()
            ,m_a, m_b);
*/
  }
}

double TimeEstimator::getTimeEstimate() const { // assume hasRegressionLine()
  const double x       = diff(m_time0, Timestamp(), TMILLISECOND);
  const double time100 = (100.0 - m_b) / m_a;
  if(x > time100) {
    return 0;
  }
  return time100 - x;
}

double TimeEstimator::getMilliSecondsRemaining() const {
  m_gate.wait();
  double result ;
  if(hasRegressionLine()) {
    result = getTimeEstimate();
  } else {
    result = 60000;
  }
  m_gate.signal();
  return result;
}
