#pragma once

#include "Runnable.h"
#include "FlagTraits.h"

#define _IRFLG_SUSPENDED   0x01
#define _IRFLG_INTERRUPTED 0x02
#define _IRFLG_TERMINATED  0x04

class InterruptableRunnable : public Runnable {
private:
  HANDLE m_thr;
  FLAGTRAITS(BYTE, InterruptableRunnable);

  InterruptableRunnable(           const InterruptableRunnable &); // not implemented. not clonable
  InterruptableRunnable &operator=(const InterruptableRunnable &); // not implemented

  inline void clrSuspended() {
    clrFlag(_IRFLG_SUSPENDED);
  }

protected:
  // The next 3 functions should only be called by this (operate on currentthread)
  // Throw Exception. If msg == NULL, exception-text is "Interrupted"
  void die(const TCHAR *msg = NULL);
  void suspend();
  inline void setTerminated() {
    setFlag(_IRFLG_TERMINATED);
  }
  virtual void handleInterruptOrSuspend();
  // if job has to be started again, use this to reset
  void clearAllFlags() {
    m_flags = 0;
  }
public:
  inline InterruptableRunnable() : m_thr(INVALID_HANDLE_VALUE), m_flags(0) {
  }
  // Mark this as interrupted. If isSuspended(), resume will be called
  void setInterrupted();

  inline void setSuspended() {
    setFlag(_IRFLG_SUSPENDED);
  }
  inline bool isSuspended() const {
    return isSet(_IRFLG_SUSPENDED);
  }
  inline bool isInterrupted() const {
    return isSet(_IRFLG_INTERRUPTED);
  }
  inline bool isTerminated() const {
    return isSet(_IRFLG_TERMINATED);
  }
  inline bool isInterruptedOrSuspended() const {
    return isSet(_IRFLG_SUSPENDED | _IRFLG_INTERRUPTED);
  }

  // Should only be called by this, or methods called directly or indirectly by this.
  // (operate on currentThread, that is the thread executing this runnable)
  inline void checkInterruptAndSuspendFlags() {
    if(isInterruptedOrSuspended()) {
      handleInterruptOrSuspend();
    }
  }
  // Should only be called by foreign thread to wake up this
  void resume();
};
