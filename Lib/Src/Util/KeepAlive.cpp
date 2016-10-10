#include "pch.h"
#include <Thread.h>

class KeepAliveThread : public Thread {
private:
  EXECUTION_STATE m_flags;
public:
  KeepAliveThread();
  void setFlags(EXECUTION_STATE flags);
  EXECUTION_STATE getFlags() const {
    return m_flags;
  }

  UINT run();
};

KeepAliveThread::KeepAliveThread() {
  setDeamon(true);
  m_flags = ES_CONTINUOUS | ES_SYSTEM_REQUIRED;
}

UINT KeepAliveThread::run() {
  for(;;) {
    setExecutionState(m_flags);
    Sleep(60000);
  }
}

void KeepAliveThread::setFlags(EXECUTION_STATE  flags) {
  m_flags = flags;
  if(flags == 0) {
    suspend();
  } else {
    resume();
  }
}

static KeepAliveThread keepAliveThread;

void Thread::keepAlive(EXECUTION_STATE flags) { // static
  keepAliveThread.setFlags(flags);
}
