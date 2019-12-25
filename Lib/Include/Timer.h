#pragma once

#include "PropertyContainer.h"
#include "FastSemaphore.h"

class Timer;
class _TimerJob;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
  virtual ~TimeoutHandler() {}
};

class Timer : private PropertyChangeListener {
private:
  const int             m_id;
  const String          m_name;
  _TimerJob            *m_job;
  mutable FastSemaphore m_lock;
  bool                  m_blockStart;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void createJob(UINT msec, TimeoutHandler &handler, bool repeatTimeout);
  void destroyJob();

public:
  Timer(int id, const String &name=EMPTYSTRING);
  virtual ~Timer();
  void startTimer(UINT msec, TimeoutHandler &handler, bool repeatTimeout = false);
  void stopTimer();
  inline int getId() const {
    return m_id;
  }
  inline const String &getName() const {
    return m_name;
  }
  void setTimeout(UINT msec, bool repeatTimeout); // do nothing, if !timer.isRunning()
  UINT getTimeout() const;
  bool isRunning() const;
};
