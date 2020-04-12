#include "pch.h"
#include <TimeEstimator.h>

TimeEstimator::TimeEstimator(const ProgressProvider &progressProvider)
: m_progressProvider(progressProvider)
, m_timer(1)
, m_timeoutCount(0)
{
  initAllSums();
  logTimeAndPct();
  m_timer.startTimer(200, *this, true);
}

TimeEstimator::~TimeEstimator() {
  m_timer.stopTimer();
}

void TimeEstimator::logTimeAndPct() {
  m_gate.wait();

  const Timestamp now;
  const double    pctDone = m_progressProvider.getPercentDone();
  const size_t    n       = m_logQueue.size();
  if(n == 0) {
    m_time0 = now;
    appendLogPoint(0, pctDone);
  } else {
    if(n >= 20) {
      removeFirst();
    }
    appendLogPoint(Timestamp::diff(m_time0, now, TMILLISECOND), pctDone);
  }
  m_gate.notify();
  switch(++m_timeoutCount) {
  case 10  : m_timer.setTimeout(1000, true); break;
  case 100 : m_timer.setTimeout(2000, true); break;
  case 1000: m_timer.setTimeout(6000, true); break;
  }
}

void TimeEstimator::appendLogPoint(double msec, double pctDone) {
  const _TimePctLogPoint p(msec, pctDone);
  m_logQueue.put(p);
  addTimePct(p);
}

void TimeEstimator::removeFirst() { // assume queue not empty
  const _TimePctLogPoint first = m_logQueue.get();
  if(first.m_msec < 800000) {
    subTimePct(first);
  } else { // avoid truncation errors if mseconds becomes to large
    m_time0.add(TMILLISECOND, (int)first.m_msec);
    initAllSums();
    for(Iterator<_TimePctLogPoint> it = m_logQueue.getIterator(); it.hasNext();) {
      _TimePctLogPoint &p = it.next();
      p.m_msec -= first.m_msec;
      addTimePct(p);
    }
  }
}

void TimeEstimator::initAllSums() {
  m_sumx = m_sumx2 = m_sumy = m_sumxy = 0;
  m_a = m_b = 0;
  m_needCalculateRegressionLine = true;
}

void TimeEstimator::addTimePct(const _TimePctLogPoint &p) {
  m_sumx  += p.m_msec;
  m_sumx2 += sqr(p.m_msec);
  m_sumy  += p.m_pctDone;
  m_sumxy += p.m_msec*p.m_pctDone;
  m_needCalculateRegressionLine = true;
}

void TimeEstimator::subTimePct(const _TimePctLogPoint &p) {
  m_sumx  -= p.m_msec;
  m_sumx2 -= sqr(p.m_msec);
  m_sumy  -= p.m_pctDone;
  m_sumxy -= p.m_msec*p.m_pctDone;
  m_needCalculateRegressionLine = true;
}

void TimeEstimator::calculateRegressionLine() const {
  const double n = (double)m_logQueue.size();
  m_needCalculateRegressionLine = false;
  if(n < 2) {
    m_a = 0; // indicates no regressionLine
    return;
  }
  const double nVarX = (m_sumx2 - sqr(m_sumx)/n);
  m_a = nVarX ? ((m_sumxy - m_sumx * m_sumy/n) / nVarX) : 0;
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
  const double x       = Timestamp::diff(m_time0, Timestamp(), TMILLISECOND);
  const double time100 = (100.0 - m_b) / m_a;
  if(x > time100) {
    return 0;
  }
  return time100 - x;
}

double TimeEstimator::getMilliSecondsRemaining() const {
  m_gate.wait();
  if(m_needCalculateRegressionLine) {
    calculateRegressionLine();
  }
  const double result = hasRegressionLine() ? getTimeEstimate() : 60000;
  m_gate.notify();
  return result;
}
