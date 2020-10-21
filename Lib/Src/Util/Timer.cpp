#include "pch.h"
#include <Date.h>
#include <ThreadPool.h>
#include <Thread.h>
#include <SafeRunnable.h>
#include <Timer.h>

#define TM_TIMEOUTREPEAT          0x01
#define TM_KILLPENDING            0x02
#define TM_BLOCKSTART             0x04
#define TM_HANDLERACTIVE          0x08
#define TM_TIMEOUTCHANGE_PENDING  0x10
#define TM_STOPTIMERCALLED        0x20
#define TM_DESTRUCTORCALLED       0x40

class _TimerJob : public SafeRunnable {
private:
  Timer            &m_timer;
  TimedSemaphore    m_timeout;
  Timestamp         m_lastTimeout;
  UINT              m_changePendingmsec; // msec
  TimeoutHandler   &m_handler;
public:
  _TimerJob(Timer *timer, TimeoutHandler &handler);
  inline bool isKilled() const {
    return m_timer.isSet(TM_KILLPENDING);
  }
  void notifyTimeout() {
    m_timeout.notify();
  }
  void timeoutChanged();
  inline UINT getTimeoutMsec() const {
    return m_timer.m_timeoutMsec;
  }
  inline bool isTimeoutRepeated() const {
    return m_timer.isSet(TM_TIMEOUTREPEAT);
  }
  UINT safeRun();
};

_TimerJob::_TimerJob(Timer *timer, TimeoutHandler &handler)
: m_timer(*timer)
, m_handler(handler)
, m_timeout(0)
{
}

void _TimerJob::timeoutChanged() {
  if(!m_timer.isSet(TM_HANDLERACTIVE)) {
    const UINT sleepTimemsec = (UINT)Timestamp::diff(m_lastTimeout, Timestamp(), TMILLISECOND);
    const UINT msec          = getTimeoutMsec();
    if(msec > sleepTimemsec) {
      m_changePendingmsec = msec - sleepTimemsec;
      m_timer.setFlag(TM_TIMEOUTCHANGE_PENDING);
    }
    notifyTimeout();
  }
}

UINT _TimerJob::safeRun() {
  m_lastTimeout = Timestamp();
  SETTHREADDESCRIPTION(format(_T("TimerJob(%s,id=%d"), m_timer.getName().cstr(), m_timer.getId()));
  while(!isKilled()) {
    m_timeout.wait(getTimeoutMsec());
    if(isKilled()) {
      break;
    }
    if(m_timer.isSet(TM_TIMEOUTCHANGE_PENDING)) {
      m_timer.clrFlag(TM_TIMEOUTCHANGE_PENDING);
      m_timeout.wait(m_changePendingmsec);
      if(isKilled()) {
        break;
      }
    }
    m_timer.setFlag(TM_HANDLERACTIVE);
    try {
      m_lastTimeout = Timestamp();
      m_handler.handleTimeout(m_timer);
      m_timer.clrFlag(TM_HANDLERACTIVE);
    } catch(...) {
      m_timer.clrFlag(TM_HANDLERACTIVE);
      throw;
    }
    if(!isTimeoutRepeated()) {
      break;
    }
  }
  return 0;
}

Timer::Timer(int id, const String &name)
: m_id(id)
, m_name(name.length() ? name : format(_T("Timer(%d)"), id))
, m_flags(0)
, m_job(nullptr)
{
  ThreadPool::addListener(this);
}

Timer::~Timer() {
  m_lock.wait();
  setFlag(TM_DESTRUCTORCALLED);
  ThreadPool::removeListener(this);
  m_lock.notify();
  destroyJob();
}

void Timer::createJob(TimeoutHandler &handler) {
  m_job = new _TimerJob(this, handler); TRACE_NEW(m_job);
  ThreadPool::executeNoWait(*m_job);
}

bool Timer::isHandlerActive() const {
  return isSet(TM_HANDLERACTIVE);
}

void Timer::destroyJob() {
  setFlag(TM_KILLPENDING);
  SAFEDELETE(m_job);
  clrFlag(TM_KILLPENDING);
}

bool Timer::isDestroyingJob() const {
  return isSet(TM_DESTRUCTORCALLED | TM_STOPTIMERCALLED | TM_KILLPENDING);
}

void Timer::startTimer(UINT msec, TimeoutHandler &handler, bool repeatTimeout) {
  if(isDestroyingJob()) return;
  m_lock.wait();
  if(!isSet(TM_BLOCKSTART)) {
    if((m_job == nullptr) || isSet(TM_KILLPENDING)) {
      destroyJob();
      m_timeoutMsec = msec;
      setFlag(TM_TIMEOUTREPEAT, repeatTimeout);
      createJob(handler);
    }
  }
  m_lock.notify();
}

void Timer::stopTimer() {
  if(isDestroyingJob()) return;
  m_lock.wait();
  setFlag(TM_STOPTIMERCALLED);
  m_lock.notify();
  destroyJob();
  clrFlag(TM_STOPTIMERCALLED);
}

bool Timer::isRunning() const {
  if(isDestroyingJob()) return false;
  m_lock.wait();
  const bool result = (m_job != nullptr);
  m_lock.notify();
  return result;
}

UINT Timer::getTimeout() const {
  if(isDestroyingJob()) return 0;
  m_lock.wait();
  const UINT result = m_job ? m_timeoutMsec : 0;
  m_lock.notify();
  return result;
}

void Timer::setTimeout(UINT msec, bool repeatTimeout) {
  if(isDestroyingJob()) return;
  m_lock.wait();
  if((m_job != nullptr) && (msec != m_timeoutMsec)) {
    m_timeoutMsec = msec;
    setFlag(TM_TIMEOUTREPEAT, repeatTimeout);
    m_job->timeoutChanged();
  }
  m_lock.notify();
}

void Timer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(ThreadPool::isPropertyContainer(source) && (id == THREADPOOL_SHUTTINGDDOWN) && *(bool*)newValue) {
    setFlag(TM_BLOCKSTART);
    stopTimer();
  }
}
