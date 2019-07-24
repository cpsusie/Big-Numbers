#include "pch.h"
#include <MyUtil.h>
#include <DebugLog.h>
#include <InputThread.h>

DEFINECLASSNAME(InputThread);

InputThread::InputThread(FILE *input, bool verbose)
: m_input(input)
, m_verbose(verbose)
, m_killed(0)
{
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
  throwException(_T("%s:Cannot kill inputThread"), __TFUNCTION__);
}

UINT InputThread::run() {
  DEFINEMETHODNAME;
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
        { String line;
          if(readLine(m_input, line)) {
            m_inputQueue.put(line);
          } else {
            m_eoi = true;
            m_inputQueue.put(EMPTYSTRING);
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
    verbose(_T("%s:%s\n"), method, e.what());
  } catch(...) {
    m_eoi = true;
    m_inputQueue.put(EMPTYSTRING);
    verbose(_T("%s:Unknown exception\n"), method);
  }
  return 0;
}

void InputThread::readTextFile(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));
  TCHAR line[1000];
  while(FGETS(line, ARRAYSIZE(line), f)) {
    m_inputQueue.put(line);
  }
  fclose(f);
}

String InputThread::getLine(int timeout) {
  const String s = m_inputQueue.get(timeout);
  if(m_eoi) {
    return EMPTYSTRING;
  }
  return s;
}

void InputThread::vverbose(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  if(m_verbose) {
    vdebugLog(format, argptr);
  }
}

void InputThread::verbose(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  va_list argptr;
  va_start(argptr, format);
  vverbose(format, argptr);
  va_end(argptr);
}
