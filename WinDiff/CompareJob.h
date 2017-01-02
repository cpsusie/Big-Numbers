#pragma once

#include <CompactArray.h>
#include <Semaphore.h>
#include <Timer.h>
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
  friend class Execute2;

private:
  CWinDiffDoc                 &m_doc;
  Execute2                    *m_exe2;
  const bool                   m_recompare;
  USHORT                       m_currentStep;
  USHORT                       m_subProgressPercent;
  String                       m_progressMessage;
  CompactArray<_ProgressStep>  m_stepArray;
  double                       m_sumEstimatedTimeUnits, m_timeUnitsDone, m_q;
  mutable Semaphore            m_gate;
  Timestamp                    m_stepStartTime;

  void updateProgressMessage();
  void setExecute2(Execute2 *exe2);
public:
  CompareJob(CWinDiffDoc *doc, bool recompare);
  ~CompareJob();

  USHORT getMaxProgress() {
    return 1000;
  }

  USHORT getProgress() {
    return (USHORT)(m_q * getMaxProgress());
  }

  USHORT getSubProgressPercent(UINT index) {
    return m_subProgressPercent;
  }

  String getProgressMessage(UINT index);

  inline const _ProgressStep &getCurrentStep() const { 
    return m_stepArray[m_currentStep];
  }

  String getTitle() {
    return _T("Comparing 2 files");
  }

  int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_SHOWPERCENT | IR_SUBPROGRESSBAR | IR_SHOWTIMEESTIMATE | IR_SHOWPROGRESSMSG | IR_INTERRUPTABLE | IR_SUSPENDABLE;
  }

  void incrProgress();

  void setSubProgressPercent(USHORT v);

  void addStep(double estimatedTimeUnits, const TCHAR *msg);
  UINT getEstimatedSecondsLeft();

  UINT run();
  void handleInterruptOrSuspend();
};

class CompareSubJob : public InterruptableRunnable {
public:
  virtual USHORT getProgressPercent() const = 0;
  virtual size_t getWeight() const = 0;
};

class Execute2 : public TimeoutHandler {
private:
  CompareJob    &m_compareJob;
  CompareSubJob *m_job[2];
public:
  Execute2(CompareJob *compareJob) : m_compareJob(*compareJob) {
  }
  void run(CompareSubJob &job1, CompareSubJob &job2);
  void handleTimeout(Timer &timer);
  void setBothSuspended();
  void setBothInterrupted();
  void resumeBoth();
};
