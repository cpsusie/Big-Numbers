#pragma once

#define LOGDEBUGINFO

#ifdef LOGDEBUGINFO

#include <Thread.h>
#include <SynchronizedQueue.h>

typedef enum {
  RESETFILE
 ,PRINTSTR
} DebugThreadCommand;

class DebugThreadQueueElement : public String {
public:
  DebugThreadCommand m_cmd;
  DebugThreadQueueElement(const String &s) : m_cmd(PRINTSTR), String(s) {
  }
  DebugThreadQueueElement(DebugThreadCommand cmd = RESETFILE) : m_cmd(cmd) {
  }
};

class DebugLogThread : public Thread {
private:
  SynchronizedQueue<DebugThreadQueueElement> m_cmdQueue;
  DebugLogThread() {
    setDeamon(true);
    start();
  }
public:
  void redirect() {
    m_cmdQueue.put(DebugThreadQueueElement());
  }
  void debugLog(const TCHAR *format, ...);
  UINT run();
  static DebugLogThread thread;
};

#define REDIRECTLOG() DebugLogThread::thread.redirect()
#define DLOG(s)       DebugLogThread::thread.debugLog s

#else

#define REDIRECTLOG()
#define DLOG(s)

#endif
