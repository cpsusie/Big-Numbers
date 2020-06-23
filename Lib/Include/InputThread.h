#pragma once

#include <Thread.h>
#include <Semaphore.h>
#include <SynchronizedQueue.h>

typedef SynchronizedQueue<String> MessageQueue;

class InputThread : public Thread {
private:
  DECLARECLASSNAME;
  MessageQueue m_inputQueue;
  bool         m_eoi;
  bool         m_verbose;
  FILE        *m_input;
  Semaphore    m_killed;
protected:
  // default implementation sends exceptions to debugLog, if m_verbose is true
  virtual void vverbose(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void          verbose(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);
public:
  InputThread(FILE *input = stdin, bool verbose = false);
  ~InputThread() override;
  // timeout in msec. throws TimeoutException on timeout
  virtual String getLine(int timeout = INFINITE);
  void kill();
  inline bool isInputReady() const {
    return !m_inputQueue.isEmpty();
  }
  inline bool endOfInput() const {
    return m_eoi;
  }
  virtual void readTextFile(const String &fileName);
  UINT run() override;
  void putMessage(const String &s) {
    m_inputQueue.put(s);
  }
  void setVerbose(bool verbose) {
    m_verbose = verbose;
  }
};
