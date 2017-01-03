#pragma once

class Runnable {
public:
  virtual UINT run() = 0;
};

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
  // the next 3 functions should only be called by this (operate on currentthread)
  void die(const TCHAR *msg = NULL);  // throw Exception. If msg = NULL, exception-text is "Interrupted"
  void suspend();
  virtual void handleInterruptOrSuspend();

public:
  inline InterruptableRunnable() : m_thr(INVALID_HANDLE_VALUE), m_flags(0) {
  }
  void setInterrupted(); // mark this as interrupted. If isSuspended(), resume will be called

  inline void setSuspended() {
    m_flags |= 1;
  }

  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline bool isSuspended() const {
    return (m_flags & 1) != 0;
  }
  inline bool isInterruptedOrSuspended() const {
    return (m_flags != 0);
  }


  // Should should only be called by this, or method called directly og indirectly by this
  // (operate on currentthread, that is the thread executing this runnable)
  inline void checkInterruptAndSuspendFlags() {
    if(isInterruptedOrSuspended()) {
      handleInterruptOrSuspend();
    }
  }
  // should only be called by foreign thread to wake up this
  void resume();
};
