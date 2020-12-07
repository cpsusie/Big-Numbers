#include "pch.h"
#include <process.h>
#include <MyUtil.h>
#include <Pipe.h>
#include <Semaphore.h>
#include <FileNameSplitter.h>
#include <ExternProcess.h>


#if defined(ENTERFUNC)
#undef ENTERFUNC
#endif
#if defined(LEAVEFUNC)
#undef LEAVEFUNC
#endif

#if defined(_DEBUG)
#define VERBOSE(msg) if(m_verbose) debugLog(_T("%*.*s%s\n"), m_level, m_level, EMPTYSTRING, msg)
#define ENTERFUNC if(m_verbose) { VERBOSE(::format(_T("enter %s"), __TFUNCTION__).cstr()); m_level++; }
#define LEAVEFUNC if(m_verbose) { m_level--;  VERBOSE(::format(_T("leave %s"), __TFUNCTION__).cstr()); }
#else
#define VERBOSE(msg)
#define ENTERFUNC
#define LEAVEFUNC
#endif

ExternProcess::ExternProcess(bool verbose) : m_verbose(verbose) {
  m_processHandle     = INVALID_HANDLE_VALUE;
  m_input = m_output  = nullptr;
  m_level = 0;
}

ExternProcess::~ExternProcess() {
  stop();
}

ArgArray::ArgArray(const String &program, const StringArray &args) {
  add(program.cstr());
  for(size_t a = 0; a < args.size(); a++) {
    add(args[a].cstr());
  }
  add(nullptr);
}

ArgArray::ArgArray(const String &program, va_list argptr) {
  add(program.cstr());
  for(TCHAR *arg = va_arg(argptr, TCHAR*);; arg = va_arg(argptr, TCHAR*)) {
    add(arg);
    if(arg == nullptr) break;
  }
  va_end(argptr);
}

String ArgArray::getCommandLine() const {
  size_t i = 0;
  String commandLine = format(_T("\"%s\""), (*this)[i++]);
  while((i < size()) && (*this)[i]) {
    commandLine += format(_T(" \"%s\""), (*this)[i++]);
  }
  return commandLine;
}

void ExternProcess::start(bool silent, const String &program, const StringArray &args) {
  ENTERFUNC;
  start(silent, ArgArray(program, args));
  LEAVEFUNC;
}

void ExternProcess::start(bool silent, const String program, ...) {
  ENTERFUNC;
  va_list argptr;
  va_start(argptr, program);
  ArgArray argv(program, argptr);
  va_end(argptr);
  start(silent, argv);
  LEAVEFUNC;
}

void ExternProcess::vstart(bool silent, const String &program, va_list argptr) {
  ENTERFUNC;
  start(silent, ArgArray(program, argptr));
  LEAVEFUNC;
}

void ExternProcess::start(bool silent, const ArgArray &argv) {
  ENTERFUNC
  Pipe oldStdFiles;
  Pipe stdinPipe, stdoutPipe;
  const String program = argv[0];

  static Semaphore critiacalSection; // juggle with filedescriptors for stdin/stdout. dont disturb!

  String oldWorkDir;

  try {
    critiacalSection.wait();

    oldWorkDir = GETCWD();

    oldStdFiles.saveStdFiles();

    stdinPipe.open();
    stdoutPipe.open();
    stdinPipe.dupAndClose( READ_FD );
    stdoutPipe.dupAndClose(WRITE_FD);

    const FileNameSplitter info(program);
    const String childWorkDir = info.getDrive() + info.getDir();
    if(childWorkDir.length()) {
      CHDIR(childWorkDir);
    }

    if(silent) {
      startCreateProcess(program, argv.getCommandLine());
    } else {
      startSpawn(program, argv.getBuffer());
    }

    CHDIR(oldWorkDir);

    m_output = stdinPipe.getFile( WRITE_FD, _T("wt"));
    m_input  = stdoutPipe.getFile(READ_FD , _T("rt"));

    oldStdFiles.restoreStdFilesAndClose();

    critiacalSection.notify();

  } catch(...) {
    oldStdFiles.restoreStdFilesAndClose();
    killProcess();
    stdinPipe.close();
    stdoutPipe.close();

    CHDIR(oldWorkDir);

    critiacalSection.notify();
    LEAVEFUNC;
    throw;
  }
  LEAVEFUNC;
}

void ExternProcess::startSpawn(const String &program, const TCHAR * const *argv) {
  HANDLE procHandle = (HANDLE)_tspawnv(_P_NOWAITO, program.cstr(), argv);
  if(procHandle != INVALID_HANDLE_VALUE) {
    setProcessHandle(procHandle);
  } else {
    throwErrNoOnSysCallException(__TFUNCTION__);
  }
}

void ExternProcess::startCreateProcess(const String &program, const String &commandLine) {
  STARTUPINFO startupInfo;
  memset(&startupInfo, 0, sizeof(startupInfo));
  startupInfo.cb          = sizeof(startupInfo);
  startupInfo.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_SHOWMINNOACTIVE;
  startupInfo.hStdInput   = (HANDLE)_get_osfhandle(0);
  startupInfo.hStdOutput  = (HANDLE)_get_osfhandle(1);
  startupInfo.hStdError   = (HANDLE)_get_osfhandle(2);

  PROCESS_INFORMATION processInfo;

  BOOL ok = CreateProcess(program.cstr(), ((String&)commandLine).cstr(), nullptr, nullptr, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo);
  if(ok) {
    setProcessHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread); // hProcess will be closed in destructor
  } else {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("CreateProcess"));
  }
}

void ExternProcess::stop() {
  ENTERFUNC;
  cleanup();
  LEAVEFUNC;
}

void ExternProcess::killProcess() {
  ENTERFUNC;
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    TerminateProcess(m_processHandle, -1);
  }
  cleanup();
  LEAVEFUNC;
}

void ExternProcess::cleanup() {
  ENTERFUNC
  if(m_output) {
    fclose(m_output);
    m_output = nullptr;
    VERBOSE(_T("m_output closed"));
  }
  if(m_input) {
    fclose(m_input);
    m_input = nullptr;
    VERBOSE(_T("m_input closed"));
  }
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    BOOL ret = CloseHandle(m_processHandle);
    setProcessHandle(INVALID_HANDLE_VALUE);
    VERBOSE(_T("processhandle closed"));
  }
  LEAVEFUNC
}

void ExternProcess::send(_In_z_ _Printf_format_string_ TCHAR const * const format,...) const {
  ENTERFUNC
  va_list argptr;
  va_start(argptr, format);
  _vftprintf(m_output, format, argptr);
  if(m_verbose) {
    const String s = vformat(format, argptr);
    VERBOSE(s.cstr());
  }
  va_end(argptr);
  fflush(m_output);
  LEAVEFUNC
}

String ExternProcess::receive() {
  ENTERFUNC
  TCHAR line[10000];
  if(!FGETS(line, ARRAYSIZE(line), m_input)) {
    line[0] = 0;
  }
  VERBOSE(line);
  LEAVEFUNC
  return line;
}

// --------------------------------------------------------------------------

int ExternProcess::runSpawn(const String &program, const TCHAR * const *argv) { // static
  HANDLE processHandle = (HANDLE)_tspawnv(_P_WAIT, program.cstr(), argv);
  if(processHandle == INVALID_HANDLE_VALUE) {
    throwErrNoOnSysCallException(__TFUNCTION__);
  }
  DWORD exitCode;
  GetExitCodeProcess(processHandle, &exitCode);
  CloseHandle(processHandle);
  return exitCode;
}

void ExternProcess::runSpawnNoWait(const String &program, const TCHAR * const *argv) { // static
  HANDLE processHandle = (HANDLE)_tspawnv(_P_NOWAIT, program.cstr(), argv);
  if(processHandle == INVALID_HANDLE_VALUE) {
    throwErrNoOnSysCallException(__TFUNCTION__);
  }
  CloseHandle(processHandle);
}

int ExternProcess::runCreateProcess(const String &program, const String &commandLine) { // static
  STARTUPINFO startupInfo;
  memset(&startupInfo, 0, sizeof(startupInfo));
  startupInfo.cb          = sizeof(startupInfo);
  startupInfo.dwFlags     = STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_SHOWMINNOACTIVE;

  PROCESS_INFORMATION processInfo;
  BOOL ok = CreateProcess(program.cstr(), ((String&)commandLine).cstr(), nullptr, nullptr, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, nullptr, nullptr, &startupInfo, &processInfo);
  DWORD exitCode;
  if(ok) {
    WaitForSingleObject(processInfo.hProcess, INFINITE);
    GetExitCodeProcess(processInfo.hProcess, &exitCode);
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
  } else {
    throwLastErrorOnSysCallException(__TFUNCTION__,_T("CreateProcess"));
  }
  return exitCode;
}

int ExternProcess::run(bool silent, const ArgArray &argv) { // static
  const String program = argv[0];
  if(silent) {
    return runCreateProcess(program, argv.getCommandLine());
  } else {
    return runSpawn(program, argv.getBuffer());
  }
}

int ExternProcess::run(bool silent, const String &program, const StringArray &args) { // static
  return run(silent, ArgArray(program, args));
}

 // wait for termination. return exit code
int ExternProcess::vrun(bool silent, const String &program, va_list argptr) { // static
  return run(silent, ArgArray(program, argptr));
}

int ExternProcess::run(bool silent, const String program, ...) { // static
  va_list argptr;
  va_start(argptr, program);
  ArgArray argv(program, argptr);
  va_end(argptr);
  return run(silent, argv);
}

void ExternProcess::runNoWait(const String program, ...) { // static
  va_list argptr;
  va_start(argptr, program);
  ArgArray argv(program, argptr);
  va_end(argptr);
  runSpawnNoWait(program, argv.getBuffer());
}
