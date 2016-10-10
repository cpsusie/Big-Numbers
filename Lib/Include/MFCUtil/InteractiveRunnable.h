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

class InteractiveRunnable : public Runnable {
private:
  char      m_flags;
  Timestamp m_jobStartTime;

  void setInterrupted() {
    m_flags |= 2;
  }
  void setSuspended() {
    m_flags |= 1;
  }
  void clrSuspended() {
    m_flags &= ~1;
  }

  friend class ProgressWindow;
  friend class CProgressDlg;
public:
  InteractiveRunnable() {
    m_flags = 0;
  }
  inline bool isSuspended() const {
    return m_flags & 1;
  }
  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline char isSuspendOrCancelButtonPressed() const {
    return m_flags;
  }
  virtual unsigned short getMaxProgress() {         // Only called if getSupportedFeatures() contains IR_PROGRESSBAR, IR_SHOWTIMEESTIMATE or IR_SUBPROGRESSBAR
    return 0;
  }
  virtual unsigned short getProgress() {            // do. Should return a short in the range [0..getMaxProgress()]
    return 0;
  };
  virtual unsigned short getSubProgressPercent() {  // Only called if getSupportedFeatures() contains IR_SUBPROGRESSBAR
    return 0;                                       // Should return an integer in the range [0..100]
  }
  virtual String getProgressMessage() {
    return "";
  }
  virtual String getTitle() {                       // Title of progress-window
    return "Progress";
  }

  virtual int getSupportedFeatures() {              // Should return any combination of IR_-constants
    return 0;
  }

  virtual UINT getEstimatedSecondsLeft();

  const Timestamp &getJobStartTime() const {
    return m_jobStartTime;
  }
};
