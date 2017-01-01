#pragma once

class Runnable {
public:
  virtual UINT run() = 0;
};

class InterruptableRunnable : public Runnable {
private:
  HANDLE m_thr;
  BYTE   m_flags;
protected:
  void die();     // throw Exception
  void suspend(); // these two should only be called by this (operate on currentthread)

  void clrSuspended() {
    m_flags &= ~1;
  }
  inline bool isInterrupted() const {
    return (m_flags & 2) != 0;
  }
  inline bool isSuspended() const {
    return m_flags & 1;
  }
  inline bool isInterruptedOrSuspended() const {
    return (m_flags != 0);
  }
public:
  inline InterruptableRunnable() : m_thr(NULL), m_flags(0) {
  }
  inline void setInterrupted() {
    m_flags |= 2;
  }
  inline void setSuspended() {
    m_flags |= 1;
  }
  void resume();
};
