#include "pch.h"
#include <Timer.h>

#define KILLED          0x01
#define REPEAT_TIMEOUT  0x02
#define HANDLER_ACTIVE  0x04

#define ISKILLED()        (m_state & KILLED         )
#define ISREPEATTIMEOUT() (m_state & REPEAT_TIMEOUT )
#define ISHANDLERACTIVE() (m_state & HANDLER_ACTIVE )

class TimerThread : private Thread {
private:
  Timer          &m_timer;
  Semaphore       m_timeout, m_stateSem;
  unsigned char   m_state;
  const int       m_msec; // msec
  TimeoutHandler &m_handler;
  void setFlag(unsigned char flag);
  void clrFlag(unsigned char flag);
public:
  TimerThread(Timer *timer, int msec, TimeoutHandler &handler, bool repeatTimeout);
  ~TimerThread();
  UINT run();
  void kill();
  inline bool isKilled() const {
    return (m_state & KILLED) != 0;
  }
  inline int getTimeout() const {
    return m_msec;
  }
};

TimerThread::TimerThread(Timer *timer, int msec, TimeoutHandler &handler, bool repeatTimeout) 
: m_timer(*timer)
, m_msec(msec)
, m_handler(handler)
, m_timeout(0)
{
  setDeamon(true);
  m_state = repeatTimeout ? REPEAT_TIMEOUT : 0;
  resume();
}

TimerThread::~TimerThread() {
  kill();
  while(stillActive()) {
    Sleep(10);
  }
}

void TimerThread::kill() {
  setFlag(KILLED);
  if(!ISHANDLERACTIVE()) {
    m_timeout.signal();
  }
}

void TimerThread::setFlag(unsigned char flags) {
  m_stateSem.wait();
  m_state |=  flags;
  m_stateSem.signal();
}

void TimerThread::clrFlag(unsigned char flags) {
  m_stateSem.wait();
  m_state &= ~flags;
  m_stateSem.signal();
}

UINT TimerThread::run() {
  while(!ISKILLED()) {
    m_timeout.wait(m_msec);

    if(ISKILLED()) {
      break;
    }

    setFlag(HANDLER_ACTIVE);
    m_handler.handleTimeout(m_timer);
    clrFlag(HANDLER_ACTIVE);

    if(!ISREPEATTIMEOUT()) {
      setFlag(KILLED);
    }
  }
  return 0;
}

Timer::Timer(int id) : m_id(id) {
  m_thread = NULL;
}

Timer::~Timer() {
  stopTimer();
  if(m_thread) {
    delete m_thread;
  }
}

void Timer::startTimer(int msec, TimeoutHandler &handler, bool repeatTimeout) {
  m_gate.wait();

  if(m_thread == NULL || m_thread->isKilled()) {
    delete m_thread;
    m_thread = NULL;
    m_thread = new TimerThread(this, msec, handler, repeatTimeout);
  }

  m_gate.signal();
}

void Timer::stopTimer() {
  m_gate.wait();

  if(m_thread) {
    m_thread->kill();
  }

  m_gate.signal();
}

bool Timer::isRunning() const {
  m_gate.wait();

  const bool result = m_thread && !m_thread->isKilled();

  m_gate.signal();
  return result;
}

int Timer::getTimeout() const {
  m_gate.wait();

  const int result = m_thread ? m_thread->getTimeout() : 0;

  m_gate.signal();
  return result;
}
