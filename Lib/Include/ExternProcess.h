#pragma once

#include <MyUtil.h>

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

  void startSpawn(             const String &program, const TCHAR * const *argv); // no wait
  void startCreateProcess(     const String &program, const String &commandLine);
  void start(                  bool silent, const ArgArray &argv);

  // return exit-code
  static int runSpawn(         const String &program, const TCHAR * const *argv); // wait until exit
  static int runCreateProcess( const String &program, const String &commandLine);
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
  ExternProcess(const ExternProcess &src);             // not defined. ExternProcess not cloneable
  ExternProcess &operator=(const ExternProcess &src);  // do
  virtual ~ExternProcess();

  // No wait
  void start( bool silent, const String &program, const StringArray &args);
  // No wait
  void vstart(bool silent, const String &program, va_list argptr);
  // No wait
  // Cannot use String &, because va_start will fail
  // Terminate argumentlist with NULL
  void start( bool silent, const String program, ...);

  // Return exit code
  static int  run(bool silent, const String &program, const StringArray &args);
  // Wait for termination
  static int vrun(bool silent, const String &program, va_list argptr);
  // Wait for termination
  // Cannot use String &, because va_start will fail
  // Terminate argumentlist with NULL
  static int  run(bool silent, const String program, ...);

  // Works only together with vstart, start.
  void stop();
  void send(const TCHAR *format, ...) const;
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
