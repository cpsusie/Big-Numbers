#pragma once

#include <MyUtil.h>
#include <Math/BigReal.h>

class BigReal;

class ExternEngine {
private:
  const String                 m_path;
  FILE                        *m_input;
  FILE                        *m_output;
  HANDLE                       m_processHandle;
  bool                         m_verbose;
  Semaphore                    m_gate;
  void stop();
  void start(bool silent, const String program, ...); // cannot use String &, because va_start will fail
  void vstartSpawn(        const String &program, va_list argptr);
  void vstartCreateProcess(const String &program, va_list argptr);
  void send(const TCHAR *format,...) const;
  void cleanup();
  void killProcess();
  String getLine();

  void setProcessHandle(HANDLE handle) {
    m_processHandle = handle;
  }
public:
  ExternEngine(const String &path);
 ~ExternEngine();
  void start();
  BigReal &mult(BigReal &dst, const BigReal &x, const BigReal &y, const BigReal &f);
  bool isStarted() const {
    return m_input != NULL;
  }
};
