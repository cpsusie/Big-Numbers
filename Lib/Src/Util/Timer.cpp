#include "pch.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <Semaphore.h>
#include <PropertyContainer.h>
#include <Timer.h>

#define KILLED                 0x01
#define REPEAT_TIMEOUT         0x02
#define HANDLER_ACTIVE         0x04
#define SETTIMEOUT_PENDING     0x08

#define ISKILLED()             (m_state & KILLED            )
#define ISREPEATTIMEOUT()      (m_state & REPEAT_TIMEOUT    )
#define ISHANDLERACTIVE()      (m_state & HANDLER_ACTIVE    )
#define ISSETTIMEOUT_PENDING() (m_state & SETTIMEOUT_PENDING)

class TimerJob : public Runnable {
private:
  Timer          &m_timer;
  FastSemaphore   m_stateSem, m_terminated;
  Semaphore       m_timeout;
  BYTE            m_state;
  int             m_msec; // msec
  TimeoutHandler &m_handler;
  void setFlag(BYTE flag);
  void clrFlag(BYTE flag);
public:
  TimerJob(Timer *timer, int msec, TimeoutHandler &handler, bool repeatTimeout);
  ~TimerJob();
  UINT run();
  void kill();
  inline bool isKilled() const {
    return (m_state & KILLED) != 0;
  }
  void setTimeout(int msec, bool repeatTimeout);
  inline int getTimeout() const {
    return m_msec;
  }
};

TimerJob::TimerJob(Timer *timer, int msec, TimeoutHandler &handler, bool repeatTimeout)
: m_timer(*timer)
, m_msec(msec)
, m_handler(handler)
, m_timeout(0)
, m_terminated(0)
{
  m_state = repeatTimeout ? REPEAT_TIMEOUT : 0;
}

TimerJob::~TimerJob() {
  kill();
  m_terminated.wait();
}

void TimerJob::kill() {
  setFlag(KILLED);
  if(!ISHANDLERACTIVE()) {
    m_timeout.notify();
  }
}

void TimerJob::setTimeout(int msec, bool repeatTimeout) {
  m_stateSem.wait();

  if(repeatTimeout) {
    m_state |= REPEAT_TIMEOUT;
  } else {
    m_state &= ~REPEAT_TIMEOUT;
  }
  m_msec = msec;
  if(!ISHANDLERACTIVE()) {
    m_state |= SETTIMEOUT_PENDING;
    m_timeout.notify();
  }
  m_stateSem.notify();
}

void TimerJob::setFlag(BYTE flags) {
  m_stateSem.wait();
  m_state |=  flags;
  m_stateSem.notify();
}

void TimerJob::clrFlag(BYTE flags) {
  m_stateSem.wait();
  m_state &= ~flags;
  m_stateSem.notify();
}

UINT TimerJob::run() {
//#ifdef _DEBUG
  setThreadDescription(m_timer.getName());
//#endif // _DEBUG

  while(!ISKILLED()) {
    m_timeout.wait(m_msec);

    if(ISKILLED()) {
      break;
    }
    if(ISSETTIMEOUT_PENDING()) {
      clrFlag(SETTIMEOUT_PENDING);
      continue;
    }
    setFlag(HANDLER_ACTIVE);
    m_handler.handleTimeout(m_timer);
    clrFlag(HANDLER_ACTIVE);

    if(!ISREPEATTIMEOUT()) {
      setFlag(KILLED);
    }
  }
  m_terminated.notify();
  return 0;
}

Timer::Timer(int id, const String &name)
: m_id(id)
, m_name(name.length()?name:format(_T("Timer(%d)"),id))
, m_blockStart(false)
{
  ThreadPool::addListener(this);
  m_job = NULL;
}

Timer::~Timer() {
  ThreadPool::removeListener(this);
  stopTimer();
  destroyJob();
}

void Timer::createJob(int msec, TimeoutHandler &handler, bool repeatTimeout) {
  m_job = new TimerJob(this, msec, handler, repeatTimeout); TRACE_NEW(m_job);
  ThreadPool::executeNoWait(*m_job);
}

void Timer::destroyJob() {
  SAFEDELETE(m_job);
}

void Timer::startTimer(int msec, TimeoutHandler &handler, bool repeatTimeout) {
  m_lock.wait();
  if(!m_blockStart) {
    if(m_job == NULL || m_job->isKilled()) {
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

int Timer::getTimeout() const {
  m_lock.wait();
  const int result = m_job ? m_job->getTimeout() : 0;
  m_lock.notify();
  return result;
}

void Timer::setTimeout(int msec, bool repeatTimeout) {
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
