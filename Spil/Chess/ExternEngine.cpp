#include "stdafx.h"
#include <fcntl.h>
#include <io.h>
#include <process.h>
#include <HashMap.h>
#include <FileVersion.h>
#include "ExternEngine.h"

#ifndef TABLEBASE_BUILDER

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
  if(!isOpen(index)) return;
  int &fd = m_fd[index];
  ::_close(fd);
  fd = -1;
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

ExternEngine::ExternEngine(const String &path) : EngineDescription(path) {
  m_processHandle     = INVALID_HANDLE_VALUE;
  m_input = m_output  = NULL;
  m_busy              = false;
  m_inputThread       = NULL;
  m_tmpGame           = NULL;
}

ExternEngine::~ExternEngine() {
  stop();
}

void ExternEngine::start() {
  start(!Options::getOptions().getShowEngineConsole(), m_path, NULL);
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

    m_inputThread = new InputThread(m_input);

    setBusy(false);

    sendUCI();

    m_tmpGame = new Game;

    critiacalSection.signal();

  } catch(Exception e) {
    oldStdFiles.restoreStdFilesAndClose();
    if(m_input  == NULL) stdinPipe.close();
    if(m_output == NULL) stdoutPipe.close();
    killProcess();

    CHDIR(oldWorkDir);

    critiacalSection.signal();

    throw e;
  }
}

void ExternEngine::stop() {
  stopSearch();

  if(m_output) {
    send(_T("quit\n"));
  }
  cleanup();
}

void ExternEngine::stopSearch() {
  if(isStarted() && isBusy()) {
    send(_T("stop\n"));
    waitUntilIdle();
  }
}

void ExternEngine::waitUntilIdle() {
  for(int i = 0; isBusy() && i < 20; i++) {
    ::Sleep(50);
  }
  if(isBusy()) {
    killProcess();
  }
}

void ExternEngine::moveNow() {
  if(isBusy()) {
    send(_T("stop\n"));
  }
}

#define INTERRUPTLINE "#interrupt#"

void ExternEngine::sendUCI() {
  m_optionArray.clear();

  String line;
  try {
    String name, author;
    while((line = getLine(2000)) != INTERRUPTLINE) {
      verbose(_T("%s\n"), line.cstr());
    }
    send(_T("uci\n"));

    Timestamp startTime;
    while((line = getLine(500)) != _T("uciok")) {
      if(diff(startTime, Timestamp(), TSECOND) > 10) {
        throwException(_T("timeout"));
      }
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext()) {
        const String command = tok.next();
        if(command == _T("id")) {
          const String s = tok.next();
          if(s == _T("name")) {
            name = tok.getRemaining();
          } else if(s == _T("author")) {
            author = tok.getRemaining();
          }
        } else if(command == _T("option")) {
          m_optionArray.add(EngineOptionDescription(tok.getRemaining(), (unsigned short)m_optionArray.size()));
        } else {
          verbose(_T("reply:<%s>\n"), line.cstr());
        }
      }
    }
    sortOptions();
    m_name   = name;
    m_author = author;
  } catch(Exception e) {
    verbose(_T("Exception <%s>. last line read:<%s\n"), e.what(), line.cstr());
    throwException(_T("Wrong or no reply on command \"uci\". Doesn't seem to be a UCI chess engine"));
  }
}

static int optionsCmp(const EngineOptionDescription &op1, const EngineOptionDescription &op2) {
  if(op1.getType() == op2.getType()) {
    return (int)op1.getIndex() - (int)op2.getIndex();
  } else {
    return op1.getType() - op2.getType();
  }
}

void ExternEngine::sortOptions() {
  m_optionArray.sort(optionsCmp);
}

EngineDescription ExternEngine::getUCIReply(const String &path) { // static
  ExternEngine engine(path);
  try {
    engine.start(true, path, NULL);
    EngineDescription desc = engine;
    engine.stop();
    return desc;
  } catch(...) {
    engine.stop();
    throw;
  }
}

void ExternEngine::killProcess() {
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    TerminateProcess(m_processHandle, -1);
  }
  cleanup();
}

#ifdef _DEBUG
#define DEBUGMSG(msg) verbose(_T("%s\n"), msg)
#else
#define DEBUGMSG(msg)
#endif

void ExternEngine::cleanup() {

  DEBUGMSG(_T("enter cleanup"));
  if(m_output) {
    fclose(m_output);
    m_output = NULL;
    DEBUGMSG(_T("m_output closed"));
  }
  if(m_input) {
    fclose(m_input);
    m_input = NULL;
    DEBUGMSG(_T("m_input closed"));
  }
  killInputThread();
  if(m_processHandle != INVALID_HANDLE_VALUE) {
    BOOL ret = CloseHandle(m_processHandle);
    setProcessHandle(INVALID_HANDLE_VALUE);
    DEBUGMSG(_T("processhandle closed"));
  }
  if(m_tmpGame != NULL) {
    delete m_tmpGame;
    m_tmpGame = NULL;
    DEBUGMSG(_T("tmpGame deleted"));
  }
  m_optionArray.clear();
  setBusy(false);
}

void ExternEngine::killInputThread() {
  DEBUGMSG(_T("enter killInputThread()"));

  if(m_inputThread == NULL) {
    DEBUGMSG(_T("inputThread already null. leaving"));
    return;
  }
  if(isBusy()) {
    DEBUGMSG(_T("putMessage(INTERRUPTLINE)"));
    m_inputThread->putMessage(INTERRUPTLINE);
  }
  for(int j = 0; j < 2; j++) {
    for(int i = 0; i < 10; i++) {
      if(!m_inputThread->stillActive()) {
        break;
      }
      Sleep(20);
    }
    if(m_inputThread->stillActive()) {
      DEBUGMSG(_T("trying to terminate process"));
      TerminateProcess(m_processHandle, -1);
      DEBUGMSG(_T("process terminated"));
    }
  }
  if(!m_inputThread->stillActive()) {
    DEBUGMSG(_T("trying to delete m_inputThread"));
    delete m_inputThread;
    DEBUGMSG(_T("m_inputThread deleted"));
  } else {
    verbose(_T("cannot kill inputthread\n"));
  }
  m_inputThread = NULL;
  DEBUGMSG(_T("m_inputThread set to NULL"));
  DEBUGMSG(_T("leaving killInputThread()"));
}

void ExternEngine::notifyGameChanged(const Game &game) {
  if(isStarted()) {
    send(_T("ucinewgame\n"));
  }
}

ExecutableMove ExternEngine::findBestMove(const Game &game, const TimeLimit &timeLimit, bool hint) {
  if(isStarted()) {
    *m_tmpGame = game;
    sendPosition(game);
    send(_T("go %s\n"), (timeLimit.m_timeout == INFINITE) ? _T("infinite") : format(_T("movetime %d"), timeLimit.m_timeout).cstr());
    setBusy(true);

    for(;;) {
      const String line = getLine();
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext()) {
        const String reply = tok.next();
        if(reply == _T("bestmove")) {
          setBusy(false);
          return game.generateMove(tok.next(), MOVE_UCIFORMAT);
        } else if(reply == INTERRUPTLINE) {
          break;
        }
      }
    }
  }
  return ExecutableMove();
}

void ExternEngine::setDebug(bool on) {
  send(_T("debug %s\n"), on ? _T("on") : _T("off"));
}

void ExternEngine::setVerbose(bool verbose) {
  m_verbose = verbose;
}

void ExternEngine::sendPosition(const Game &game) const {
  if(isStarted()) {
    send(_T("%s"), game.toUCIString().cstr());
  }
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, int value) {
  assert(option.getType() == OptionTypeSpin);
  if(isStarted()) {
    send(_T("setoption name %s value %d\n"), option.getName().cstr(), value);
  }
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, bool value) {
  assert(option.getType() == OptionTypeCheckbox);
  if(isStarted()) {
    if(&option == &EngineOptionDescription::debugOption) {
      setDebug(value);
    } else {
      send(_T("setoption name %s value %s\n"), option.getName().cstr(), value ? _T("true") : _T("false"));
    }
  }
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, const String &value) {
  assert((option.getType() == OptionTypeCombo) || (option.getType() == OptionTypeString));
  if(isStarted()) {
    send(_T("setoption name %s value %s\n"), option.getName().cstr(), value.cstr());
  }
}

void ExternEngine::clickButton(const EngineOptionDescription &option) {
  assert(option.getType() == OptionTypeButton);
  if(isStarted()) {
    send(_T("setoption name %s\n"), option.getName().cstr());
  }
}

void ExternEngine::setParameters(const EngineOptionValueArray &valueArray) {
  for(Iterator<EngineOptionValue> it = valueArray.getIterator(); it.hasNext();) {
    setParameterValue(it.next());
  }
}

void ExternEngine::setParameterValue(const EngineOptionValue &v) {
  const EngineOptionDescription *optionDesc = m_optionArray.findOptionByName(v.getName());
  if(optionDesc == NULL) {
    AfxMessageBox(format(_T("Option %s not found for engine %s"), v.getName().cstr(), getName().cstr()).cstr(), MB_ICONWARNING);
    return;
  }
  switch(optionDesc->getType()) {
  case OptionTypeSpin    :
    setParameterValue(*optionDesc, v.getIntValue());
    break;

  case OptionTypeCombo   :
  case OptionTypeString  :
    setParameterValue(*optionDesc, v.getStringValue());
    break;
  case OptionTypeCheckbox:
    setParameterValue(*optionDesc, v.getBoolValue());
    break;
  }
}

void ExternEngine::send(const TCHAR *format,...) const {
  va_list argptr;
#ifdef _DEBUG
  va_start(argptr, format);
  String line = vformat(format, argptr);
  va_end(argptr);
  line.replace('\n',_T(""));
  verbose(_T("send <%s>\n"), line.cstr());
  USES_CONVERSION;
  const char *aline = T2A(line.cstr());
  fprintf(m_output, "%s\n", aline);
#else
  va_start(argptr, format);
  const String line = vformat(format, argptr);
  va_end(argptr);
  USES_CONVERSION;
  const char *aline = T2A(line.cstr());
  fprintf(m_output, "%s", aline);
#endif
  fflush(m_output);
}

class EngineInfoLine {
public:
  int              m_depth;
  int              m_seldepth;
  String           m_score;
  int              m_time;    // milliseconds
  unsigned __int64 m_nodes;
  UINT             m_nodesps;
  String           m_pv;
  String           m_string;
  int              m_hashFull;
  int              m_multiPV;
  int              m_cpuLoad;

  EngineInfoLine() {
    reset();
  }
  EngineInfoLine &operator+=(const String &line);
  String toString(const EngineVerboseFields &evf) const;
  void reset();
  bool isReady() const {
    return m_score.length() > 0;
  }
};

String ExternEngine::getLine(int milliseconds) {
  const EngineVerboseFields &evf = getOptions().getengineVerboseFields();
  try {
    EngineInfoLine infoLine;
    for(;;) {
      if(!isStarted()) {
        setBusy(false);
        throwException(_T("Unexpected eof. Extern engine is dead"));
      }
      String line = m_inputThread->getLine(milliseconds);
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext() && tok.next() == _T("info")) {
        if(m_verbose) {
          infoLine += tok.getRemaining();
          if(infoLine.isReady()) {
            verbose(_T("%s"), infoLine.toString(evf).cstr());
            if(evf.m_pv && (infoLine.m_pv.length() > 0)) {
              updateMessageField(_T("%s"), getBeautifiedVariant(infoLine.m_pv).cstr());
            }
            infoLine.reset();
          }
        }
      } else {
        return line;
      }
    }
  } catch(Exception e) {
    verbose(_T("getLine:Exception:%s\n"), e.what());
    return INTERRUPTLINE;
  }
}

String ExternEngine::getBeautifiedVariant(const String &pv) const {
  const int startPly    = m_tmpGame->getPlyCount();
  int       moveCounter = startPly/2 + 1;
  String    result;
  if(m_tmpGame->getPlayerInTurn() == BLACKPLAYER) {
    result = format(_T("%d. -"), moveCounter);
  }
  try {
    for(Tokenizer tok(pv, _T(" ")); tok.hasNext();) {
      const ExecutableMove m = m_tmpGame->generateMove(tok.next(), MOVE_UCIFORMAT);
      if(m_tmpGame->getPlayerInTurn() == WHITEPLAYER) {
        result += format(_T("%d. %s"), moveCounter, m.toString().cstr());
      } else {
        result += format(_T(", %s "), m.toString().cstr());
        moveCounter++;
      }
      m_tmpGame->doMove(m_tmpGame->generateMove(m));
    }
  } catch(Exception e) {
    result += e.what();
  }
  while(m_tmpGame->getPlyCount() > startPly) {
    m_tmpGame->undoMove();
  }
  return result;
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
    m_path = program;
  } else {
    throwErrNoOnSysCallException(_T("_tspawnv"));
  }
}

void ExternEngine::vstartCreateProcess(const String &program, va_list argptr) {
  String commandLine = format(_T("\"%s\""), program.cstr());
  for(TCHAR *arg = va_arg(argptr, TCHAR*); arg; arg = va_arg(argptr, TCHAR*)) {
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
    m_path = program;
  } else {
    throwException(_T("createProcess %s failed. %s"), program.cstr(), getLastErrorText().cstr());
  }
}

String ExternEngine::toString() const {
  String result = format(_T("Started     :%s\n"
                            "Busy        :%s\n")
                         ,boolToStr(isStarted())
                         ,boolToStr(isBusy())
                        );
  if(isStarted()) {
    if(m_optionArray.size() > 0) {
      result += _T("Options:\n");
      for(size_t i = 0; i < m_optionArray.size(); i++) {
        const EngineOptionDescription &option = m_optionArray[i];
        result += format(_T("  %s\n"), option.toString().cstr());
      }
    }
  }
  return result;
}

// --------------------------------------EngineOptionDescription---------------------------------------------

const EngineOptionDescription EngineOptionDescription::debugOption(_T("name Debug type check default false"),999);

EngineOptionDescription::EngineOptionDescription(String line, unsigned short index) : m_index(index) {
  DEFINEMETHODNAME;
  m_min = m_max = m_defaultInt = 0;
  m_defaultBool = false;
  String defaultStr;
  bool typeDefined = false;

  int nameIndex = line.find(_T("name"));
  if(nameIndex < 0) {
    throwInvalidArgumentException(method, _T("No name for option. line:<%s>"), line.cstr());
  }
  line = substr(line, nameIndex+5, line.length());
  for(Tokenizer tok1(line, _T(" ")); tok1.hasNext();) {
    const String word = tok1.next();
    if(word == _T("type") || word == _T("default") || word == _T("min") || word == _T("max") || word == _T("var")) {
      int nextField = line.find(word);
      m_name = trim(substr(line, 0, nextField - 1));
      line = substr(line, nextField, line.length());
      break;
    }
  }

  for(Tokenizer tok(line, _T(" ")); tok.hasNext();) {
    const String field = tok.next();
    if(field == _T("type")) {
      const String typeStr = tok.next();
      if(typeStr == _T("check")) {
        m_type = OptionTypeCheckbox;
      } else if(typeStr == _T("spin")) {
        m_type = OptionTypeSpin;
      } else if(typeStr == _T("combo")) {
        m_type = OptionTypeCombo;
      } else if(typeStr == _T("button")) {
        m_type = OptionTypeButton;
      } else if(typeStr == _T("string")) {
        m_type = OptionTypeString;
      } else {
        throwInvalidArgumentException(method, _T("Invalid type:<%s>"), typeStr.cstr());
      }
      typeDefined = true;
    } else if(field == _T("min")) {
      m_min = tok.getInt();
    } else if(field == _T("max")) {
      m_max = tok.getInt();
    } else if(field == _T("default")) {
      if(m_type == OptionTypeString) {
        defaultStr = tok.getRemaining();
        break;
      } else {
        defaultStr = tok.next();
      }
    } else if(field == _T("var")) {
      m_comboValues.add(tok.next());
    } else {
      throwInvalidArgumentException(method, _T("Invalid field:<%s>"), field.cstr());
    }
  }
  if(!typeDefined) {
    throwInvalidArgumentException(method, _T("Type not defined. line:<%s>"), line.cstr());
  }
  switch(m_type) {
  case OptionTypeCheckbox :
    if(defaultStr == _T("false")) {
      m_defaultBool = false;
    } else if(defaultStr == _T("true")) {
      m_defaultBool = true;
    } else {
      throwInvalidArgumentException(method, _T("Invalid default value for check option %s:<%s>")
                                   ,m_name.cstr(), defaultStr.cstr());
    }
    break;
  case OptionTypeSpin     :
    if(m_min > m_max) {
      throwInvalidArgumentException(method, _T("Invalid range for spin option %s. min=%d, max=%d")
                                   ,m_name.cstr(), m_min, m_max);
    }
    if(_stscanf(defaultStr.cstr(), _T("%d"), &m_defaultInt) != 1) {
      throwInvalidArgumentException(method, _T("Invalid default value for spin option %s:<%s>")
                                   ,m_name.cstr(), defaultStr.cstr());
    }
    if(m_defaultInt < m_min || m_max < m_defaultInt) {
      throwInvalidArgumentException(method, _T("Invalid default value for spin option %s:%d. Range:[%d;%d]")
                                   ,m_name.cstr(), m_defaultInt, m_min,m_max);
    }
    break;
  case OptionTypeButton   :
    break;
  case OptionTypeCombo    :
    if(m_comboValues.size() == 0) {
      throwInvalidArgumentException(method, _T("No values defined for combo option %s. line:<%s>"), m_name.cstr(), line.cstr());
    }
    // NB continue case
  case OptionTypeString   :
    m_defaultString = defaultStr;
    break;
  }
}

int EngineOptionDescription::getValueIndex(const String &str) const {
  for(size_t i = 0; i < m_comboValues.size(); i++) {
    if(m_comboValues[i] == str) {
      return i;
    }
  }
  return -1;
}

String EngineOptionDescription::getLabelName() const {
  String result = m_name;
  return result.replace('_',' ');
}

String EngineOptionDescription::toString() const {
  switch(m_type) {
  case OptionTypeCheckbox: return format(_T("%-20s Checkbox default:%s")                 , m_name.cstr(), boolToStr(m_defaultBool));
  case OptionTypeSpin    : return format(_T("%-20s Spin     default:%6d  range:[%d..%d]"), m_name.cstr(), m_defaultInt, m_min, m_max);
  case OptionTypeCombo   : return format(_T("%-20s Combobox default:<%s> values:[%s]")   , m_name.cstr(), m_defaultString.cstr(), m_comboValues.toString().cstr());
  case OptionTypeButton  : return format(_T("%-20s Button")                              , m_name.cstr());
  case OptionTypeString  : return format(_T("%-20s String   default:<%s>")               , m_name.cstr(), m_defaultString.cstr());
  }
  return format(_T("%s:Unknown type:%d"), m_name.cstr(), m_type);
}

String EngineOptionDescription::getDefaultAsString() const {
  switch(m_type) {
  case OptionTypeCheckbox: return getDefaultBool() ? _T("Checked") : _T("Unchecked");
  case OptionTypeSpin    : return format(_T("%d"), getDefaultInt());
  case OptionTypeCombo   :
  case OptionTypeString  : return getDefaultString();
  case OptionTypeButton  :
  default                : return _T("");
  }
}

const EngineOptionDescription *EngineOptionDescriptionArray::findOptionByName(const String &optionName) const {
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &option = (*this)[i];
    if(option.getName() == optionName) {
      return &option;
    }
  }
  return NULL;
}

EngineOptionValueArray EngineOptionDescriptionArray::createDefaultValueArray(const String &engineName) const {
  EngineOptionValueArray result(engineName);
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionDescription &desc = (*this)[i];
    switch(desc.getType()) {
    case OptionTypeCheckbox: result.setValue(desc.getName(), desc.getDefaultBool());   break;
    case OptionTypeSpin    : result.setValue(desc.getName(), desc.getDefaultInt());    break;
    case OptionTypeCombo   :
    case OptionTypeString  : result.setValue(desc.getName(), desc.getDefaultString()); break;
    }
  }
  return result;
}

EngineOptionDescriptionArray &EngineOptionDescriptionArray::removeOptionsByType(EngineOptionType type) { // return *this
  for(int i = size(); i--;) {
    if((*this)[i].getType() == type) {
      removeIndex(i);
    }
  }
  return *this;
}

EngineOptionValueArray EngineOptionDescriptionArray::pruneDefaults(const EngineOptionValueArray &valueArray) const {
  EngineOptionValueArray result(valueArray);
  for(int i = result.size(); i--;) {
    const EngineOptionValue       &v      = result[i];
    const EngineOptionDescription *option = findOptionByName(v.getName());
    bool prune = false;
    if(option == NULL) {
      prune = true;
    } else {
      switch(option->getType()) {
      case OptionTypeSpin    :
        if(v.getIntValue() == option->getDefaultInt()) {
          prune = true;
        }
        break;
      case OptionTypeCombo   :
      case OptionTypeString  :
        if(v.getStringValue() == option->getDefaultString()) {
          prune = true;
        }
        break;
      case OptionTypeCheckbox:
        if(v.getBoolValue() == option->getDefaultBool()) {
          prune = true;
        }
        break;
      default:
        prune = true;
        break;
      }
    }
    if(prune) {
      result.removeIndex(i);
    }
  }
  return result;
}

typedef enum {
  ENGINE_DEPTH
 ,ENGINE_SELDEPTH
 ,ENGINE_SCORE
 ,ENGINE_LOWERBOUND
 ,ENGINE_UPPERBOUND
 ,ENGINE_TIME
 ,ENGINE_NODES
 ,ENGINE_NODESPS
 ,ENGINE_PV
 ,ENGINE_STRING
 ,ENGINE_HASHFULL
 ,ENGINE_MULTIPV
 ,ENGINE_CPULOAD
} EngineInfoField;

class InfoHashMap : public StrHashMap<EngineInfoField> {
public:
  InfoHashMap();
};

InfoHashMap::InfoHashMap() {
  put(_T("depth"     ), ENGINE_DEPTH      );
  put(_T("seldepth"  ), ENGINE_SELDEPTH   );
  put(_T("score"     ), ENGINE_SCORE      );
  put(_T("lowerbound"), ENGINE_LOWERBOUND );
  put(_T("upperbound"), ENGINE_UPPERBOUND );
  put(_T("time"      ), ENGINE_TIME       );
  put(_T("nodes"     ), ENGINE_NODES      );
  put(_T("nps"       ), ENGINE_NODESPS    );
  put(_T("pv"        ), ENGINE_PV         );
  put(_T("string"    ), ENGINE_STRING     );
  put(_T("hashfull"  ), ENGINE_HASHFULL   );
  put(_T("multipv"   ), ENGINE_MULTIPV    );
  put(_T("cpuload"   ), ENGINE_CPULOAD    );
}

void EngineInfoLine::reset() {
  m_pv = m_score = m_string = _T("");
  m_depth    = 0;
  m_seldepth = 0;
  m_time     = 0;
  m_nodes    = 0;
  m_nodesps  = 0;
  m_hashFull = 0;
  m_cpuLoad  = 0;
}

EngineInfoLine &EngineInfoLine::operator+=(const String &line) {
  static InfoHashMap keywords;

  for(Tokenizer tok(line, _T(" ")); tok.hasNext();) {
    EngineInfoField *field = keywords.get(tok.next().cstr());
    if(field == NULL) {
      continue;
    }
    switch(*field) {
    case ENGINE_DEPTH   :
      m_depth = tok.getInt();
      break;
    case ENGINE_SELDEPTH   :
      m_seldepth = tok.getInt();
      break;
    case ENGINE_SCORE   :
      { const String s = tok.next();
        if(s == _T("cp")) {
          m_score = _T("=") + tok.next();
        } else if(s == _T("mate")) {
          const int moves = tok.getInt();
          if(moves > 0) {
            m_score = format(_T("Mate in %d"), moves);
          } else {
            m_score = format(_T("Loose in %d"), -moves);
          }
        }
      }
      break;
    case ENGINE_LOWERBOUND:
      if(m_score.length()) m_score[0] = _T('>');
      break;
    case ENGINE_UPPERBOUND:
      if(m_score.length()) m_score[0] = _T('<');
      break;
    case ENGINE_TIME    :
      m_time = tok.getInt();
      break;
    case ENGINE_NODES   :
      m_nodes = tok.getInt64();
      break;
    case ENGINE_NODESPS :
      m_nodesps = tok.getUint();
      break;
    case ENGINE_PV      :
      m_pv = tok.getRemaining();
      return *this;
    case ENGINE_STRING:
      m_string = tok.getRemaining();
      return *this;
    case ENGINE_HASHFULL:
      m_hashFull = tok.getInt();
      break;
    case ENGINE_MULTIPV :
      m_multiPV = tok.getInt();
      break;
    case ENGINE_CPULOAD :
      m_cpuLoad = tok.getInt();
      break;
    }
  }
  return *this;
}

String EngineInfoLine::toString(const EngineVerboseFields &evf) const {
  String result;
  if(m_score.length() > 0) {
    if(evf.m_depth   ) result += format(_T("depth:%2d "     ), m_depth                     );
    if(evf.m_seldepth) result += format(_T("seldepth:%2d "  ), m_seldepth                  );
    if(evf.m_time    ) result += format(_T("time:%6.2lf "   ), (double)m_time/1000         );
    if(evf.m_nodes   ) result += format(_T("nodes:%13s "    ), format1000(m_nodes  ).cstr());
    if(evf.m_nodesps ) result += format(_T("nodes/sec:%10s "), format1000(m_nodesps).cstr());
    if(evf.m_score   ) result += format(_T("score%-6s "     ), m_score.cstr()              );
    if(evf.m_cpuLoad ) result += m_cpuLoad  ? format(_T("CPU:%4.1lf%% "     ), (double)m_cpuLoad/10.0) : _T("          ");
    if(evf.m_hashfull) result += m_hashFull ? format(_T("HashLoad:%4.1lf%% "), (double)m_hashFull/10.0) : _T("               ");
    if(result.length()) result += _T("\n");
  }
  if(evf.m_string && (m_string.length() > 0)) {
    result += m_string;
    result += _T("\n");
  }
  return result;
}

#endif // TABLEBASE_BUILDER
