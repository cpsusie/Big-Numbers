#pragma once

#include "PropertyContainer.h"
#include "FastSemaphore.h"

class Timer;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
  virtual ~TimeoutHandler() {}
};

class Timer : private PropertyChangeListener {
  friend class TimerJob;
private:
  const int             m_id;
  const String          m_name;
  TimerJob             *m_job;
  mutable FastSemaphore m_lock;
  bool                  m_blockStart;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void createJob(int msec, TimeoutHandler &handler, bool repeatTimeout);
  void destroyJob();

public:
  Timer(int id, const String &name=EMPTYSTRING);
  virtual ~Timer();
  void startTimer(int msec, TimeoutHandler &handler, bool repeatTimeout = false);
  void stopTimer();
  inline int getId() const {
    return m_id;
  }
  inline const String &getName() const {
    return m_name;
  }
  void setTimeout(int msec, bool repeatTimeout); // do nothing, if !timer.isRunning()
  int  getTimeout() const;
  bool isRunning() const;
};
