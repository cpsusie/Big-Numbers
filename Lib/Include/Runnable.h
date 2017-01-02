#pragma once

class Runnable {
public:
  virtual UINT run() = 0;
};

class InterruptableRunnable : public Runnable {
private:
  HANDLE m_thr;
  BYTE   m_flags;
  void clrSuspended() {
    m_flags &= ~1;
  }
protected:
  void die();     // throw Exception
  void suspend(); // these two should only be called by this (operate on currentthread)

  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline bool isSuspended() const {
    return (m_flags & 1) != 0;
  }
  inline bool isInterruptedOrSuspended() const {
    return (m_flags != 0);
  }
public:
  inline InterruptableRunnable() : m_thr(INVALID_HANDLE_VALUE), m_flags(0) {
  }
  void setInterrupted();
  inline void setSuspended() {
    m_flags |= 1;
  }
  void resume(); // should only be called by foreign thread to wake up this
                 // if it's suspended.
                 // If setInterrupted is called while *this is suspended, resume
                 // will be called, so it can commit suicide by calling die.
};
