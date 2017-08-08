#pragma once

#include "Runnable.h"

class InterruptableRunnable : public Runnable {
private:
  HANDLE m_thr;
  BYTE   m_flags;

  InterruptableRunnable(           const InterruptableRunnable &); // not implemented. not clonable
  InterruptableRunnable &operator=(const InterruptableRunnable &); // not implemented

  void clrSuspended() {
    m_flags &= ~1;
  }

protected:
  // The next 3 functions should only be called by this (operate on currentthread)
  // Throw Exception. If msg == NULL, exception-text is "Interrupted"
  void die(const TCHAR *msg = NULL);
  void suspend();
  inline void setTerminated() {
    m_flags |= 4;
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
    m_flags |= 1;
  }

  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline bool isSuspended() const {
    return (m_flags & 1) != 0;
  }
  inline bool isTerminated() const {
    return (m_flags & 4) != 0;
  }
  inline bool isInterruptedOrSuspended() const {
    return (m_flags&3) != 0;
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
