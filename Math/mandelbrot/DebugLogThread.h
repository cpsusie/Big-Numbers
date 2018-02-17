#pragma once

//#define LOGDEBUGINFO

#ifdef LOGDEBUGINFO

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
  void debugLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  UINT run();
  static DebugLogThread thread;
};

#define REDIRECTLOG() DebugLogThread::thread.redirect()
#define DLOG(...)     DebugLogThread::thread.debugLog(__VA_ARGS__)

#else

#define REDIRECTLOG()
#define DLOG(...)

#endif
