#include "stdafx.h"
#include "InputThread.h"

InputThread::InputThread(FILE *input) : m_input(input) {
  setDeamon(true);
  m_eoi = false;
  resume();
}

unsigned int InputThread::run() {
  try {
#ifdef _DEBUG
    if(!isatty(stdout)) {
      replyMessage(_T("InputThread:Id:%d"), getThreadId());
    }
#endif
    for(;;) {
      TCHAR line[1000];
      try {
        if(!FGETS(line, ARRAYSIZE(line), m_input)) {
          m_eoi = true;
          m_inputQueue.put(EMPTYSTRING);
          return 0;
        } else {
  #ifdef _DEBUG
          if(!isatty(stdout)) {
            replyMessage(_T("read from stdin:<%s>"), line);
          }
  #endif
          strTrim(line);
          if(_tcsicmp(line, _T("quit")) == 0) {
            m_eoi = true;
            m_inputQueue.put(EMPTYSTRING);
            return 0;
          }
          Tokenizer tok(line, _T(" "));
          if(!tok.hasNext()) {
            continue;
          }
          if(tok.next() == _T("read") && tok.hasNext()) {
            readFile(tok.next());
            continue;
          }
          m_inputQueue.put(line);
        }
      } catch(Exception e) {
        reply(_T("Exception:%s\n.\n"), e.what());
      }
    }
    return 0;
  } catch(...) {
    m_eoi = true;
    m_inputQueue.put(EMPTYSTRING);
    reply(_T("unknown Exception in inputthread\n.\n"));
    return 0;
  }
}

void InputThread::readFile(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));
  TCHAR buffer[1000];
  while(FGETS(buffer,ARRAYSIZE(buffer),f)) {
    m_inputQueue.put(buffer);
  }
  fclose(f);
}

String InputThread::getLine() {
  const String s = m_inputQueue.get();
  if(m_eoi) {
    return EMPTYSTRING;
  }
  return s;
}

bool InputThread::isInputReady() {
  return !m_inputQueue.isEmpty();
}
