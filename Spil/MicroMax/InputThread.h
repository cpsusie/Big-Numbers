#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>

typedef SynchronizedQueue<String> MessageQueue;

class InputThread : public Thread {
private:
  MessageQueue m_inputQueue;
  bool         m_eoi;
  FILE        *m_input;
  void readFile(const String &fileName);
public:
  InputThread(FILE *input = stdin);
  String getLine();
  bool isInputReady();
  bool endOfInput() const {
    return m_eoi;
  }
  unsigned int run();
  void putMessage(const String &s) {
    m_inputQueue.put(s);
  }
};
