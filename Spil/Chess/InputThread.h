#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>

typedef SynchronizedQueue<String> MessageQueue;

class InputThread : public Thread {
private:
  MessageQueue m_inputQueue;
  bool         m_eoi;
  FILE        *m_input;
public:
  InputThread(FILE *input = stdin);
  String getLine(int milliseconds = INFINITE); // throws TimeoutException on timeout
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
};
