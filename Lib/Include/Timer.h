#pragma once

#include "Thread.h"
#include "PropertyContainer.h"

class Timer;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
  virtual ~TimeoutHandler() {}
};

class Timer : private PropertyChangeListener {
  friend class TimerThread;
private:
  const int             m_id;
  TimerThread          *m_thread;
  mutable FastSemaphore m_gate;
  bool                  m_blockStart;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void createThread(int msec, TimeoutHandler &handler, bool repeatTimeout);
  void destroyThread();

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
