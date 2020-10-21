#pragma once

#include "SafeRunnable.h"

#define _IRFLG_SUSPENDED   0x01
#define _IRFLG_INTERRUPTED 0x02

class InterruptableRunnable : public SafeRunnable {
private:
  // m_thr == INVALID_HANDLE_VALUE, unless suspend() is called, and the running thread is temporarily stopped
  // Set to INVALID_HANDLE_VALUE, when thread is resumed
  HANDLE m_thr;
  FLAGTRAITS(InterruptableRunnable, BYTE, m_flags);

  InterruptableRunnable(           const InterruptableRunnable &); // not implemented. not clonable
  InterruptableRunnable &operator=(const InterruptableRunnable &); // not implemented

  inline InterruptableRunnable &clrSuspended() {
    return clrFlag(_IRFLG_SUSPENDED);
  }
  void saveHandle();
  InterruptableRunnable &clearHandle();
  inline bool isPaused() const {
    return m_thr != INVALID_HANDLE_VALUE;
  }
protected:
  // Should only be called by this (operate on currentthread)
  // Throw Exception. If msg == nullptr, exception-text is "Interrupted"
  void die(const TCHAR *msg = nullptr);
  // Should only be called by this (operate on currentthread)
  // Save currentThreadHandle in m_thr, and call SuspendThread(m_thr)
  // When resume is called, clearHandle() and clrSuspended() are called
  // If isInterrupted(), die() is called, or else thread continues execution
  // Only thread itself should call this function, may cause deadlocks if not use with care
  void suspend();
  // Should only be called by this (operate on currentthread)
  virtual void handleInterruptOrSuspend();
  // if job has to be started again, use this to reset
  void clearAllFlags() {
    m_flags = 0;
  }
public:
  inline InterruptableRunnable() : m_thr(INVALID_HANDLE_VALUE), m_flags(0) {
  }
  // Mark this as interrupted. If isPaused(), resume will be called
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
  // Do nothing unless isPaused() == true
  void resume();
};
