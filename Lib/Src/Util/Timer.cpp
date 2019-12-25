#include "pch.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <Date.h>
#include <Semaphore.h>
#include <PropertyContainer.h>
#include <Timer.h>

#define KILLED                 0x01
#define REPEAT_TIMEOUT         0x02
#define HANDLER_ACTIVE         0x04
#define CHANGETIMEOUT_PENDING  0x08

#define ISKILLED()                (m_flags & KILLED               )
#define ISREPEATTIMEOUT()         (m_flags & REPEAT_TIMEOUT       )
#define ISHANDLERACTIVE()         (m_flags & HANDLER_ACTIVE       )
#define ISCHANGETIMEOUT_PENDING() (m_flags & CHANGETIMEOUT_PENDING)

#define setFlag(fl) m_flags |= (fl)
#define clrFlag(fl) m_flags &= ~(fl)

class _TimerJob : public Runnable {
private:
  Timer            &m_timer;
  FastSemaphore     m_terminated;
  Semaphore         m_timeout;
  std::atomic<BYTE> m_flags;
  Timestamp         m_lastTimeout;
  UINT              m_msec, m_changePendingmsec; // msec
  TimeoutHandler   &m_handler;
public:
  _TimerJob(Timer *timer, UINT msec, TimeoutHandler &handler, bool repeatTimeout);
  ~_TimerJob();
  UINT run();
  void kill();
  inline bool isKilled() const {
    return ISKILLED();
  }
  void setTimeout(UINT msec, bool repeatTimeout);
  inline UINT getTimeout() const {
    return m_msec;
  }
};

_TimerJob::_TimerJob(Timer *timer, UINT msec, TimeoutHandler &handler, bool repeatTimeout)
: m_timer(*timer)
, m_msec(msec)
, m_handler(handler)
, m_timeout(0)
{
  m_flags = repeatTimeout ? REPEAT_TIMEOUT : 0;
}

_TimerJob::~_TimerJob() {
  kill();
  m_terminated.wait();
}

void _TimerJob::kill() {
  setFlag(KILLED);
  if(!ISHANDLERACTIVE()) {
    m_timeout.notify();
  }
}

void _TimerJob::setTimeout(UINT msec, bool repeatTimeout) {
  if(repeatTimeout) {
    setFlag(REPEAT_TIMEOUT);
  } else {
    clrFlag(REPEAT_TIMEOUT);
  }
  if(msec == m_msec) return;
  m_msec = msec;
  if(!ISHANDLERACTIVE()) {
    const UINT sleepTimemsec = (UINT)Timestamp::diff(m_lastTimeout, Timestamp(), TMILLISECOND);
    if(msec > sleepTimemsec) {
      m_changePendingmsec = msec - sleepTimemsec;
      setFlag(CHANGETIMEOUT_PENDING);
    }
    m_timeout.notify();
  }
}

UINT _TimerJob::run() {
  m_terminated.wait();
  m_lastTimeout = Timestamp();
  try {
#ifdef _DEBUG
    setThreadDescription(m_timer.getName());
#endif // _DEBUG

    while(!ISKILLED()) {
      m_timeout.wait(m_msec);
      if(ISKILLED()) break;
      if(ISCHANGETIMEOUT_PENDING()) {
        clrFlag(CHANGETIMEOUT_PENDING);
        m_timeout.wait(m_changePendingmsec);
        if(ISKILLED()) break;
      }
      setFlag(HANDLER_ACTIVE);
      try {
        m_lastTimeout = Timestamp();
        m_handler.handleTimeout(m_timer);
        clrFlag(HANDLER_ACTIVE);
      } catch(...) {
        clrFlag(HANDLER_ACTIVE);
        throw;
      }
      if(!ISREPEATTIMEOUT()) setFlag(KILLED);
    }
  } catch(...) {
    // ignore
  }
  m_terminated.notify();
  return 0;
}

Timer::Timer(int id, const String &name)
: m_id(id)
, m_name(name.length()?name:format(_T("Timer(%d)"),id))
, m_blockStart(false)
, m_job(NULL)
{
  ThreadPool::addListener(this);
}

Timer::~Timer() {
  ThreadPool::removeListener(this);
  stopTimer();
  destroyJob();
}

void Timer::createJob(UINT msec, TimeoutHandler &handler, bool repeatTimeout) {
  m_job = new _TimerJob(this, msec, handler, repeatTimeout); TRACE_NEW(m_job);
  ThreadPool::executeNoWait(*m_job);
}

void Timer::destroyJob() {
  SAFEDELETE(m_job);
}

void Timer::startTimer(UINT msec, TimeoutHandler &handler, bool repeatTimeout) {
  m_lock.wait();
  if(!m_blockStart) {
    if((m_job == NULL) || m_job->isKilled()) {
      destroyJob();
      createJob(msec, handler, repeatTimeout);
    }
  }
  m_lock.notify();
}

void Timer::stopTimer() {
  m_lock.wait();
  if(m_job) {
    m_job->kill();
  }
  m_lock.notify();
}

bool Timer::isRunning() const {
  m_lock.wait();
  const bool result = m_job && !m_job->isKilled();
  m_lock.notify();
  return result;
}

UINT Timer::getTimeout() const {
  m_lock.wait();
  const UINT result = m_job ? m_job->getTimeout() : 0;
  m_lock.notify();
  return result;
}

void Timer::setTimeout(UINT msec, bool repeatTimeout) {
  m_lock.wait();
  if(m_job && !m_job->isKilled()) {
    m_job->setTimeout(msec, repeatTimeout);
  }
  m_lock.notify();
}

void Timer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(ThreadPool::isPropertyContainer(source) && (id == THREADPOOL_SHUTTINGDDOWN)) {
    m_blockStart = true;
    stopTimer();
    destroyJob();
  }
}
