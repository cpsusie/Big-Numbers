#pragma once

#include <Date.h>
#include <Runnable.h>
#include "WinTools.h"

#define IR_PROGRESSBAR        0x01
#define IR_SUBPROGRESSBAR     0x02
#define IR_INTERRUPTABLE      0x04
#define IR_SUSPENDABLE        0x08
#define IR_SHOWTIMEESTIMATE   0x10
#define IR_SHOWPROGRESSMSG    0x20
#define IR_SHOWPERCENT        0x40

class InteractiveRunnable : public InterruptableRunnable {
private:
  Timestamp m_jobStartTime;

  USHORT getAvgSubProgressPercent();

  friend class ProgressWindow;
  friend class CProgressDlg;
public:
  inline bool isSuspendOrCancelButtonPressed() const {
    return isInterruptedOrSuspended();
  }
  virtual USHORT getMaxProgress() {                    // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
    return 0;
  }
  virtual USHORT getProgress() {                       // do. Should return a short in the range [0..getMaxProgress()]
    return 0;
  };
  virtual USHORT getJobCount() const {
    return 1;
  }
  virtual USHORT getSubProgressPercent(UINT index=0) { // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
    return 0;                                          // Should return an integer in the range [0..100]
  }
  virtual String getProgressMessage(UINT index=0) {
    return _T("");
  }
  virtual String getTitle() {                          // Title of progress-window
    return _T("Progress");
  }
  virtual int getSupportedFeatures() {                 // Should return any combination of IR_-constants
    return 0;
  }
  virtual UINT getEstimatedSecondsLeft();

  const Timestamp &getJobStartTime() const {
    return m_jobStartTime;
  }
};
