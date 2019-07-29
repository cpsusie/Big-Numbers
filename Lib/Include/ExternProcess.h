#pragma once

#include "CompactArray.h"
#include "StringArray.h"

class ArgArray : public CompactArray<const TCHAR*> {
public:
  ArgArray(const String &program, const StringArray &args);
  ArgArray(const String &program, va_list argptr);
  String getCommandLine() const;
};

class ExternProcess {
private:
  FILE                        *m_input;
  FILE                        *m_output;
  HANDLE                       m_processHandle;
  bool                         m_verbose;
  mutable short                m_level;

  ExternProcess(const ExternProcess &src);             // Not defined. Class not cloneable
  ExternProcess &operator=(const ExternProcess &src);  // Not defined. Class not cloneable

  // No wait. Can communicate with process' stdin/stdout by send/receive
  void startSpawn(             const String &program, const TCHAR * const *argv);
  // No wait. Can communicate with process' stdin/stdout by send/receive
  void startCreateProcess(     const String &program, const String &commandLine);
  // No wait. Can communicate with process' stdin/stdout by send/receive
  void start(                  bool silent, const ArgArray &argv);

  // Wait for termination. Return exit-code
  static int runSpawn(         const String &program, const TCHAR * const *argv);
  // Wait for termination. Return exit-code
  static int runCreateProcess( const String &program, const String &commandLine);
  // Wait for termination. Return exit-code
  static int run(              bool silent, const ArgArray &argv);

  void cleanup();

  void setProcessHandle(HANDLE handle) {
    m_processHandle = handle;
  }
protected:
  void killProcess();
  inline FILE *getInput() {
    return m_input;
  }
  inline FILE *getOutput() {
    return m_output;
  }
public:
  // If verbose, functioncalls will be traced with debugLog
  ExternProcess(bool verbose = false);
  virtual ~ExternProcess();

  // No wait. Can communicate with process' stdin/stdout by send/receive
  void start( bool silent, const String &program, const StringArray &args);
  // No wait. Can communicate with process' stdin/stdout by send/receive
  void vstart(bool silent, const String &program, va_list argptr);
  // No wait. Can communicate with process' stdin/stdout by send/receive
  // Cannot use String &, because va_start will fail
  // Terminate argumentlist with NULL
  void start( bool silent, const String program, ...);

  // Wait for termination. Return exit-code
  static int  run(bool silent, const String &program, const StringArray &args);
  // Wait for termination. Return exit-code
  static int vrun(bool silent, const String &program, va_list argptr);
  // Wait for termination. Return exit-code
  // Cannot use String &, because va_start will fail
  // Terminate argumentlist with NULL
  static int  run(bool silent, const String program, ...);

  // Works only together with vstart, start.
  void stop();
  // Works only together with vstart, start. Send strings can be read on stdin by child-process
  void send(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const;
  // Works only together with vstart, start. Can receive text written to stdout by child-process
  String receive();
  inline bool isStarted() const {
    return m_input != NULL;
  }
  inline void setVerbose(bool verbose) {
    m_verbose = verbose;
  }
  inline bool isVerbose() const {
    return m_verbose;
  }
};
