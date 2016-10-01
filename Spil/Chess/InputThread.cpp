#include "stdafx.h"
#include "InputThread.h"

InputThread::InputThread(FILE *input) : m_input(input) {
  setDeamon(true);
  m_eoi = false;
  resume();
}

UINT InputThread::run() {
  try {
    for(;;) {
      TCHAR line[1000];
      if(!FGETS(line, ARRAYSIZE(line), m_input)) {
        m_eoi = true;
        m_inputQueue.put(_T(""));
        return 0;
      } else {
        strTrim(line);
        if(_tcsicmp(line, _T("quit")) == 0) {
          m_eoi = true;
          m_inputQueue.put(_T(""));
          return 0;
        }
        m_inputQueue.put(line);
      }
    }
  } catch(Exception e) {
    m_eoi = true;
    m_inputQueue.put(_T(""));
    verbose(_T("Exception in inputthread:%s\n.\n"), e.what());
    return 0;
  } catch(...) {
    m_eoi = true;
    m_inputQueue.put(_T(""));
    verbose(_T("unknown Exception in inputthread\n.\n"));
    return 0;
  }
}

void InputThread::readFile(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));
  TCHAR line[1000];
  while(FGETS(line, ARRAYSIZE(line), f)) {
    m_inputQueue.put(line);
  }
  fclose(f);
}

String InputThread::getLine(int milliseconds) {
  const String s = m_inputQueue.get(milliseconds);
  if(m_eoi) {
    return _T("");
  }
  return s;
}
