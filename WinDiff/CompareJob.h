#pragma once

#include <CompactArray.h>
#include <Semaphore.h>
#include <MFCUtil/InteractiveRunnable.h>

class CWinDiffDoc;

class _ProgressStep {
public:
  double       m_timeUnits;
  const TCHAR *m_msg;

#ifdef MEASURE_STEPTIME
  Timestamp   m_startTime;
  Timestamp   m_endTime;
  double getMeasuredTime() const {
    return diff(m_startTime, m_endTime, TMILLISECOND);
  }
#endif

  _ProgressStep() : m_timeUnits(0), m_msg(_T("")) {
  }
  _ProgressStep(double CPUUnits, const TCHAR *msg) : m_timeUnits(CPUUnits), m_msg(msg) {
  }
};

class CompareJob : public InteractiveRunnable {
private:
  CWinDiffDoc &m_doc;

  const bool                   m_recompare;
  unsigned short               m_currentStep;
  unsigned short               m_subProgressPercent;
  String                       m_progressMessage;
  CompactArray<_ProgressStep>  m_stepArray;
  double                       m_sumEstimatedTimeUnits, m_timeUnitsDone, m_q;
  Semaphore                    m_sem;
  Timestamp                    m_stepStartTime;

  void updateProgressMessage();

public:
  CompareJob(CWinDiffDoc *doc, bool recompare);
  ~CompareJob();

  unsigned short getMaxProgress() {
    return 1000;
  }

  unsigned short getProgress() {
    return (unsigned short)(m_timeUnitsDone / m_sumEstimatedTimeUnits * 1000);
  }

  unsigned short getSubProgressPercent() {
    return m_subProgressPercent;
  }

  String getProgressMessage();

#ifdef MEASURE_STEPTIME
  const _ProgressStep &getCurrentStep() const { 
    return m_stepArray[m_currentStep];
  }
#endif

  String getTitle() {
    return _T("Comparing 2 files");
  }

  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_SHOWPERCENT | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE | IR_SHOWPROGRESSMSG | IR_INTERRUPTABLE | IR_SUSPENDABLE;
  }

  void incrProgress();

  void setSubProgressPercent(unsigned short v);

  void addStep(double estimatedTimeUnits, const TCHAR *msg);

  UINT getEstimatedSecondsLeft();

  UINT run();
};
