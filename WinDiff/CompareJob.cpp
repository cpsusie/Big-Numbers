#include "stdafx.h"
#include "CompareJob.h"
#include "WinDiffDoc.h"

CompareJob::CompareJob(CWinDiffDoc *doc, bool recompare) 
: m_doc(*doc)
, m_recompare(recompare)
{
  m_sumEstimatedTimeUnits = 0;
  addStep(0,_T(""));
}

#ifdef MEASURE_STEPTIME
static double getTotalTime(const CompactArray<_ProgressStep>  &a) {
  double sum = 0;
  for(int i = 1; i < a.size(); i++) {
    sum += a[i].getMeasuredTime();
  }
  return sum;
}
#endif

CompareJob::~CompareJob() {
#ifdef MEASURE_STEPTIME
  Timestamp now;
  m_stepArray[m_currentStep].m_endTime = now;
  const double totalTime = getTotalTime(m_stepArray);
  for(int i = 1; i < m_stepArray.size(); i++) {
    const  _ProgressStep &s = m_stepArray[i];
    double measurePct       = PERCENT(s.getMeasuredTime(),totalTime              );
    double estimatePct      = PERCENT(s.m_timeUnits      ,m_sumEstimatedTimeUnits);
    debugLog(_T("%-30s:Measured %%:%4.1lf Estimated %%:%4.1lf\n"), s.m_msg, measurePct, estimatePct);
  }
#endif
}

String CompareJob::getProgressMessage(UINT index) {
  m_sem.wait();
  String result = m_progressMessage;
  m_sem.signal();
  return result;
}

void CompareJob::updateProgressMessage() {
  m_sem.wait();
#ifdef MEASURE_STEPTIME
  m_progressMessage = format(_T("Step %2d/%2d (q=%.1lf%%:%s")
                            ,m_currentStep, m_stepArray.size()-1
                            ,m_q*100
                            ,m_stepArray[m_currentStep].m_msg);
#else
  m_progressMessage = format(_T("%s"), m_stepArray[m_currentStep].m_msg);
#endif

  m_sem.signal();
}

void CompareJob::incrProgress() {
  if(m_currentStep >= m_stepArray.size()) {
    return;
  }
#ifdef MEASURE_STEPTIME
  const Timestamp now;
  m_stepArray[m_currentStep].m_endTime = now;
#endif

  m_timeUnitsDone += m_stepArray[m_currentStep++].m_timeUnits;

#ifdef MEASURE_STEPTIME
  m_stepArray[m_currentStep].m_startTime = now;
#endif

  m_subProgressPercent = 0;
  updateProgressMessage();
}

void CompareJob::setSubProgressPercent(USHORT v) {
  m_subProgressPercent = min(v,100);
  if(isSuspendOrCancelButtonPressed()) {
    if(isInterrupted()) {
      throwException(_T("Compare cancelled by user"));
    } else if(isSuspended()) {
      SuspendThread(GetCurrentThread());
    }
  }
}

void CompareJob::addStep(double estimatedTimeUnits, const TCHAR *msg) {
  m_stepArray.add(_ProgressStep(estimatedTimeUnits, msg));
  m_sumEstimatedTimeUnits += estimatedTimeUnits;
}

UINT CompareJob::getEstimatedSecondsLeft() {
  const double q = (m_timeUnitsDone + m_stepArray[m_currentStep].m_timeUnits * m_subProgressPercent/100) / m_sumEstimatedTimeUnits;

  if(q == 0) {
    return 10;
  } else if(q >= 1) {
    return 0;
  }
  m_q = q;
  const double secondsUsed = diff(getJobStartTime(), Timestamp(), TSECOND);

  return (UINT)(secondsUsed / q * (1-q));
}

UINT CompareJob::run() {
  try {
    m_currentStep         = 0;
    m_subProgressPercent  = 0;
    m_timeUnitsDone       = 0;
    m_q                   = 0;
    updateProgressMessage();

    if(m_recompare) {
      m_doc.m_diff.compare(m_doc.m_filter, m_doc.m_cmp, this);
    } else {
      m_doc.m_diff.refreshLines(this);
    }
  } catch(Exception e) {
    showException(e);
    m_doc.m_diff.clear();
  }
  return 0;
}

