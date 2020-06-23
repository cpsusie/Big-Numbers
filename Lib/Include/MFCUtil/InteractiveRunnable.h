#pragma once

#include <Date.h>
#include <InterruptableRunnable.h>
#include <TimeEstimator.h>
#include "WinTools.h"

#define IR_PROGRESSBAR        0x01
#define IR_SUBPROGRESSBAR     0x02
#define IR_INTERRUPTABLE      0x04
#define IR_SUSPENDABLE        0x08
#define IR_SHOWTIMEESTIMATE   0x10
#define IR_AUTOCORRELATETIME  0x20
#define IR_SHOWPROGRESSMSG    0x40
#define IR_SHOWPERCENT        0x80

class InteractiveRunnable : public InterruptableRunnable, public ProgressProvider {
  friend class ProgressWindow;
private:
  Timestamp      m_jobStartTime;
  TimeEstimator *m_timeEstimator;
  // Set startTime til now, and if neccessary, allocate m_timeEstimator
  const Timestamp &setStartTime();
  void cleanup();
protected:
  double getAvgSubProgressPercent();

public:
  InteractiveRunnable() : m_timeEstimator(NULL) {
  }
 ~InteractiveRunnable() override {
    cleanup();
  }
  inline bool isSuspendOrCancelButtonPressed() const {
    return isInterruptedOrSuspended();
  }
  // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
  double getMaxProgress() const override {
    return 0;
  }
  // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
  // Should return a value in the range [0..getMaxProgress()]
  double getProgress() const override {
    return 0;
  };
  virtual USHORT getJobCount() const {
    return 1;
  }
  // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
  // Should return an integer in the range [0..100]
  virtual USHORT getSubProgressPercent(UINT index=0) {
    return 0;
  }
  virtual String getProgressMessage(UINT index=0) {
    return EMPTYSTRING;
  }
  // Title of progress-window
  virtual String getTitle() {
    return _T("Progress");
  }
  // Should return any combination of IR_-constants
  virtual int getSupportedFeatures() {
    return 0;
  }
  double getSecondsRemaining() const {
    return m_timeEstimator ? (m_timeEstimator->getMilliSecondsRemaining()/1000) : 0;
  }

  const Timestamp &getJobStartTime() const {
    return m_jobStartTime;
  }
};
