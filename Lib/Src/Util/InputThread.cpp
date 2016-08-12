#include "pch.h"
#include <InputThread.h>

DEFINECLASSNAME(InputThread);

InputThread::InputThread(FILE *input, bool verbose) : m_input(input), m_verbose(verbose), m_killed(0) {
  setDeamon(true);
  m_eoi = false;
  resume();
}

InputThread::~InputThread() {
  if (stillActive()) {
    kill();
  }
}

void InputThread::kill() {
  m_killed.signal();
  for (int i = 0; i < 10; i++) {
    if (!stillActive()) {
      return;
    }
    Sleep(100);
  }
  throwException(_T("%s:Cannot kill inputThread"), _T(__FUNCTION__));
}

unsigned int InputThread::run() {
  HANDLE handles[2];
  handles[0] = m_killed.getHandle();
  handles[1] = getHandle(m_input);
  try {
    while(!m_eoi) {
      DWORD waitResult = WaitForMultipleObjects(ARRAYSIZE(handles), handles, FALSE, INFINITE);
      switch (waitResult) {
      case WAIT_OBJECT_0 + 0: // m_killed is signaled
        m_eoi = true;
        m_inputQueue.put(EMPTYSTRING);
        continue;
      case WAIT_OBJECT_0 + 1: // input file has something for us
        { TCHAR line[4096];
          if (!FGETS(line, ARRAYSIZE(line), m_input)) {
            m_eoi = true;
            m_inputQueue.put(EMPTYSTRING);
          }
          else {
            m_inputQueue.put(line);
          }
        }
        break;
      case WAIT_FAILED:
        throwMethodLastErrorOnSysCallException(s_className, _T("WaitForMultipleObjects"));
      }
    }
  } catch(Exception e) {
    m_eoi = true;
    m_inputQueue.put(EMPTYSTRING);
    if(m_verbose) debugLog(_T("Exception in inputthread:%s\n"), e.what());
  } catch(...) {
    m_eoi = true;
    m_inputQueue.put(EMPTYSTRING);
    if(m_verbose) debugLog(_T("unknown Exception in inputthread\n"));
  }
  return 0;
}

void InputThread::readFile(const String &fileName) {
  FILE *f = FOPEN(fileName, "r");
  TCHAR line[1000];
  while(FGETS(line, ARRAYSIZE(line), f)) {
    m_inputQueue.put(line);
  }
  fclose(f);
}

String InputThread::getLine(int timeoutInMilliseconds) {
  const String s = m_inputQueue.get(timeoutInMilliseconds);
  if(m_eoi) {
    return EMPTYSTRING;
  }
  return s;
}
