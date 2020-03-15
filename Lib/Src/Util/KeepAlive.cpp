#include "pch.h"
#include <Thread.h>
#include <Timer.h>

class KeepAliveTrigger : public TimeoutHandler {
private:
  EXECUTION_STATE m_flags;
  Timer           m_timer;
public:
  KeepAliveTrigger();
  void setFlags(EXECUTION_STATE flags);
  EXECUTION_STATE getFlags() const {
    return m_flags;
  }
  void handleTimeout(Timer &timer);
};

KeepAliveTrigger::KeepAliveTrigger() : m_timer(1,_T("Keep Alive")) {
  m_flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED;
}

void KeepAliveTrigger::handleTimeout(Timer &timer) {
  Thread::setExecutionState(m_flags);
}

void KeepAliveTrigger::setFlags(EXECUTION_STATE  flags) {
  m_flags = flags;
  if(flags == 0) {
    m_timer.stopTimer();
  } else {
    m_timer.startTimer(6000, *this, true);
  }
}

void Thread::keepAlive(EXECUTION_STATE flags) { // static
  static KeepAliveTrigger keepAliveTrigger;
  keepAliveTrigger.setFlags(flags);
}
