#pragma once

#include "Thread.h"

class Timer;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
  virtual ~TimeoutHandler() {}
};

class TimerThread;

class Timer {
private:
  const int             m_id;
  TimerThread          *m_thread;
  mutable FastSemaphore m_gate;
public:
  Timer(int id);
  virtual ~Timer();
  void startTimer(int msec, TimeoutHandler &handler, bool repeatTimeout = false);
  void stopTimer();
  inline int getId() const {
    return m_id;
  }
  void setTimeout(int msec, bool repeatTimeout); // do nothing, if !timer.isRunning()
  int  getTimeout() const;
  bool isRunning() const;
};
