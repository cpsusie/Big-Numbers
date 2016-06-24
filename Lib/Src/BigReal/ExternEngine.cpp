#include "pch.h"
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <HashMap.h>
#include <Math/BigReal.h>

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

ExternEngine::ExternEngine(const String &path) : m_path(path) {
  m_processHandle     = INVALID_HANDLE_VALUE;
  m_input = m_output  = NULL;
}

ExternEngine::~ExternEngine() {
  stop();
}

void ExternEngine::start() {
  start(true, m_path, NULL);
}

void ExternEngine::start(bool silent, const String program,...) {
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

    va_list argptr;
    va_start(argptr, program);
    if(silent) {
      vstartCreateProcess(program, argptr);
    } else {
      vstartSpawn(program, argptr);
    }
    va_end(argptr);

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
    throw e;
  }
}

void ExternEngine::stop() {
  cleanup();
}

void ExternEngine::killProcess() {
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    TerminateProcess(m_processHandle, -1);
  }
  cleanup();
}

#ifdef _DEBUG
#define DEBUGMSG(msg) _tprintf(_T("%s\n"), _T(msg))
#else
#define DEBUGMSG(msg)
#endif

void ExternEngine::cleanup() {
  DEBUGMSG("enter cleanup");
  if(m_output) {
    fclose(m_output);
    m_output = NULL;
    DEBUGMSG("m_output closed");
  }
  if(m_input) {
    fclose(m_input);
    m_input = NULL;
    DEBUGMSG("m_input closed");
  }
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    BOOL ret = CloseHandle(m_processHandle);
    setProcessHandle(INVALID_HANDLE_VALUE);
    DEBUGMSG("processhandle closed");
  }
}

BigReal &ExternEngine::mult(BigReal &dst, const BigReal &x, const BigReal &y, const BigReal &f) {
  DigitPool *pool = dst.getDigitPool();
  FullFormatBigReal xf(x, pool), yf(y, pool), ff(f, pool);
  BigRealStream xs, ys, fs;
  xs << xf;
  ys << yf;
  fs << ff;

  m_gate.wait();
  try {
    send(_T("%s %s %s"), xs.cstr(), ys.cstr(), fs.cstr());
    const String line = getLine();
    dst = BigReal(line, pool);
  }
  catch (Exception e) {
    _tprintf(_T("Exception:%s\n"), e.what());
    dst = 0;
  }
  catch (...) {
    _tprintf(_T("Unknown exception\n"));
    dst = 0;
  }
  m_gate.signal();
  return dst;
}

void ExternEngine::send(const TCHAR *format,...) const {
  va_list argptr;
#ifdef _DEBUG
  va_start(argptr, format);
  String line = vformat(format, argptr);
  va_end(argptr);
  line.replace('\n',"");
  _tprintf(_T("send <%s>\n"), line.cstr());
  _ftprintf(m_output, _T("%s\n"), line.cstr());
#else
  va_start(argptr, format);
  _vftprintf(m_output, format, argptr);
  va_end(argptr);
#endif
  fflush(m_output);
}

String ExternEngine::getLine() {
  try {
    TCHAR line[100000];
    if (!FGETS(line, ARRAYSIZE(line), m_input)) {
      return _T("");
    }
    strTrim( line);
    _tprintf(_T("  Received:<%s>\n"), line);
    return line;
  } catch(Exception e) {
    _tprintf(_T("getLine:Exception:%s\n"), e.what());
    return _T("error");
  }
}

void ExternEngine::vstartSpawn(const String &program, va_list argptr) {
  const TCHAR *args[100];
  int i = 0;
  args[i++] = program.cstr();
  for(TCHAR *arg = va_arg(argptr,TCHAR*); arg && i < ARRAYSIZE(args)-1; arg = va_arg(argptr,TCHAR*)) {
    args[i++] = arg;
  }
  va_end(argptr);
  args[i] = NULL;

  HANDLE procHandle = (HANDLE)_tspawnv(_P_NOWAITO, program.cstr(), args);
  if(procHandle != INVALID_HANDLE_VALUE) {
    setProcessHandle(procHandle);
  } else {
    throwException(_T("spawn %s failed. %s"), program.cstr(), _sys_errlist[errno]);
  }
}

void ExternEngine::vstartCreateProcess(const String &program, va_list argptr) {
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
    CloseHandle(processInfo.hThread); // hProcess will be closed when extern engine dies
  } else {
    throwException(_T("createProcess %s failed. %s"), program.cstr(), getLastErrorText().cstr());
  }
}

