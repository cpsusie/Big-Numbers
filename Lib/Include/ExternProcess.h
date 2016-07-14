#pragma once

#include <MyUtil.h>

class ExternProcess {
private:
  DECLARECLASSNAME;
  FILE                        *m_input;
  FILE                        *m_output;
  HANDLE                       m_processHandle;
  bool                         m_verbose;
  mutable short                m_level;

  void vstartSpawn(        const String &program, va_list argptr);
  void vstartCreateProcess(const String &program, va_list argptr);
  void cleanup();
  void killProcess();

  void setProcessHandle(HANDLE handle) {
    m_processHandle = handle;
  }
public:
  ExternProcess(bool verbose = false);                 // if verbose, functioncalls will be traced with debugLog
  ExternProcess(const ExternProcess &src);             // not defined. ExternProcess not cloneable
  ExternProcess &operator=(const ExternProcess &src);  // do
 ~ExternProcess();
  void vstart(bool silent, const String &program, va_list argptr);
  void start( bool silent, const String program, ...); // cannot use String &, because va_start will fail
                                                       // terminate argumentlist with NULL
  void stop();
  void send(const TCHAR *format, ...) const;
  String receive();
  inline bool isStarted() const {
    return m_input != NULL;
  }
  void setVerbose(bool verbose) {
    m_verbose = verbose;
  }
};
