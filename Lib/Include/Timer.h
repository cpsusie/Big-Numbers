#pragma once

#include "PropertyContainer.h"
#include <FlagTraits.h>
#include "FastSemaphore.h"

class Timer;

class TimeoutHandler {
public:
  virtual void handleTimeout(Timer &timer) = 0;
  virtual ~TimeoutHandler() {}
};

class Timer : private PropertyChangeListener {
  friend class _TimerJob;
private:
  const int             m_id;
  const String          m_name;
  UINT                  m_timeoutMsec;
  ATOMICFLAGTRAITS(BYTE,Timer)
  _TimerJob            *m_job;
  mutable FastSemaphore m_lock;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  // no lock protection
  void createJob(TimeoutHandler &handler);
  // no lock protection
  void destroyJob();
  bool isDestroyingJob() const;
  bool isHandlerActive() const;
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
  // do nothing, if !timer.isRunning()
  void setTimeout(UINT msec, bool repeatTimeout);
  UINT getTimeout() const;
  bool isRunning() const;
};
