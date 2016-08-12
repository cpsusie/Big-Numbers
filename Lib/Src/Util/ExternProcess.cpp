#include "pch.h"
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <Semaphore.h>
#include <ExternProcess.h>

#define READ_FD  0
#define WRITE_FD 1

class Pipe {
private:
  int m_fd[2];
  void init();
  void close(int index);
public:
  Pipe() { init(); }
  Pipe(int bufferSize, int textMode);
  void open(int bufferSize = 256, int textMode = _O_TEXT | _O_NOINHERIT);
  void close();
  void dupAndClose(int index);
  void saveStdFiles();
  void restoreStdFilesAndClose();
  bool isOpen(int index) const {
    return m_fd[index] != -1;
  }
  FILE *getFile(int index, const char *mode) {
    assert(isOpen(index));
    return _fdopen(m_fd[index], mode);
  }
};

Pipe::Pipe(int bufferSize, int textMode) {
  init();
  open(bufferSize, textMode);
}

void Pipe::open(int bufferSize, int textMode) {
  close();
  PIPE(m_fd,bufferSize, textMode);
}

void Pipe::dupAndClose(int index) {
  assert(isOpen(index));
  DUP2(m_fd[index], index);
  close(index);
}

void Pipe::close() {
  close(READ_FD);
  close(WRITE_FD);
}

void Pipe::close(int index) {
  int &fd = m_fd[index];
  if(fd != -1) { 
    ::_close(fd);
    fd = -1;
  }
}

void Pipe::saveStdFiles() {
  close();
  m_fd[0] = DUP(0);
  m_fd[1] = DUP(1);
}

void Pipe::restoreStdFilesAndClose() {
  for(int i = 0; i < 2; i++) {
    int fd = m_fd[i];
    if(fd != -1) {
      DUP2(fd, i);
      close(i);
    }
  }
}

void Pipe::init() {
  m_fd[0] = m_fd[1] = -1;
}

#define VERBOSE(msg) if(m_verbose) debugLog(_T("%*.*s%s\n"), m_level, m_level, _T(""), msg)

#define ENTERFUNC if(m_verbose) { VERBOSE(::format(_T("enter %s"), _T(__FUNCTION__)).cstr()); m_level++; }
#define EXITFUNC if(m_verbose) { m_level--;  VERBOSE(::format(_T("exit  %s"), _T(__FUNCTION__)).cstr()); }

ExternProcess::ExternProcess(bool verbose) : m_verbose(verbose) {
  m_processHandle     = INVALID_HANDLE_VALUE;
  m_input = m_output  = NULL;
  m_level = 0;
}

ExternProcess::~ExternProcess() {
  stop();
}

void ExternProcess::start(bool silent, const String program, ...) {
  ENTERFUNC
  va_list argptr;
  va_start(argptr, program);
  try {
    vstart(silent, program, argptr);
    va_end(argptr);
  }
  catch (...) {
    va_end(argptr);
    EXITFUNC;
    throw;
  }
  EXITFUNC;
}

void ExternProcess::vstart(bool silent, const String &program, va_list argptr) {
  ENTERFUNC
  Pipe oldStdFiles;
  Pipe stdinPipe, stdoutPipe;

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

    FileNameSplitter info(program);
    String childWorkDir = info.getDrive() + info.getDir();
    CHDIR(childWorkDir);

    if(silent) {
      vstartCreateProcess(program, argptr);
    } else {
      vstartSpawn(program, argptr);
    }

    CHDIR(oldWorkDir);

    m_output = stdinPipe.getFile( WRITE_FD, "wt");
    m_input  = stdoutPipe.getFile(READ_FD , "rt");

    oldStdFiles.restoreStdFilesAndClose();

    critiacalSection.signal();

  } catch(Exception e) {
    oldStdFiles.restoreStdFilesAndClose();
    killProcess();
    stdinPipe.close();
    stdoutPipe.close();

    CHDIR(oldWorkDir);

    critiacalSection.signal();
    EXITFUNC;
    throw e;
  }
  EXITFUNC;
}

void ExternProcess::stop() {
  ENTERFUNC;
  cleanup();
  EXITFUNC;
}

void ExternProcess::killProcess() {
  ENTERFUNC;
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    TerminateProcess(m_processHandle, -1);
  }
  cleanup();
  EXITFUNC;
}

void ExternProcess::cleanup() {
  ENTERFUNC
  if(m_output) {
    fclose(m_output);
    m_output = NULL;
    VERBOSE("m_output closed");
  }
  if(m_input) {
    fclose(m_input);
    m_input = NULL;
    VERBOSE("m_input closed");
  }
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    BOOL ret = CloseHandle(m_processHandle);
    setProcessHandle(INVALID_HANDLE_VALUE);
    VERBOSE("processhandle closed");
  }
  EXITFUNC
}

void ExternProcess::send(const TCHAR *format,...) const {
  ENTERFUNC
  va_list argptr;
  va_start(argptr, format);
  _vftprintf(m_output, format, argptr);
  if (m_verbose) {
    const String s = vformat(format, argptr);
    VERBOSE(s.cstr());
  }
  va_end(argptr);
  fflush(m_output);
  EXITFUNC
}

String ExternProcess::receive() {
  ENTERFUNC
  TCHAR line[10000];
  if (!FGETS(line, ARRAYSIZE(line), m_input)) {
    line[0] = 0;
  }
  VERBOSE(line);
  EXITFUNC
  return line;
}

void ExternProcess::vstartSpawn(const String &program, va_list argptr) {
  const TCHAR *argv[100];
  int i = 0;
  argv[i++] = program.cstr();
  for(TCHAR *arg = va_arg(argptr,TCHAR*); arg && i < ARRAYSIZE(argv)-1; arg = va_arg(argptr,TCHAR*)) {
    argv[i++] = arg;
  }
  va_end(argptr);
  argv[i] = NULL;

  HANDLE procHandle = (HANDLE)_tspawnv(_P_NOWAITO, program.cstr(), argv);
  if(procHandle != INVALID_HANDLE_VALUE) {
    setProcessHandle(procHandle);
  } else {
    throwErrNoOnSysCallException(_T("_tspawnv"));
  }
}

void ExternProcess::vstartCreateProcess(const String &program, va_list argptr) {
  String commandLine = format(_T("\"%s\""), program.cstr());
  for(char *arg = va_arg(argptr, char*); arg; arg = va_arg(argptr, char*)) {
    commandLine += format(_T(" \"%s\""), arg);
  }

  STARTUPINFO startupInfo;
  memset(&startupInfo, 0, sizeof(startupInfo));
  startupInfo.cb          = sizeof(startupInfo);
  startupInfo.dwFlags     = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
  startupInfo.wShowWindow = SW_SHOWMINNOACTIVE;
  startupInfo.hStdInput   = (HANDLE)_get_osfhandle(0);
  startupInfo.hStdOutput  = (HANDLE)_get_osfhandle(1);
  startupInfo.hStdError   = (HANDLE)_get_osfhandle(2);

  PROCESS_INFORMATION processInfo;

  BOOL ok = CreateProcess(program.cstr(), commandLine.cstr(), NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS | CREATE_NO_WINDOW, NULL, NULL, &startupInfo, &processInfo);
  if(ok) {
    setProcessHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread); // hProcess will be closed in destructor
  } else {
    throwLastErrorOnSysCallException(_T(__FUNCTION__));
  }
}

