#pragma once

#include <Thread.h>
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
public:
  InputThread(FILE *input = stdin, bool verbose = false);
  virtual ~InputThread();
  String getLine(int timeoutInMilliseconds = INFINITE); // throws TimeoutException on timeout
  void kill();
  inline bool isInputReady() const {
    return !m_inputQueue.isEmpty();
  }
  inline bool endOfInput() const {
    return m_eoi;
  }
  void readFile(const String &fileName);
  unsigned int run();
  void putMessage(const String &s) {
    m_inputQueue.put(s);
  }
  void setVerbose(bool verbose) {
    m_verbose = verbose;
  }
};
