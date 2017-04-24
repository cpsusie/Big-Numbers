#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include "ExternEngine.h"

#ifdef ENTERFUNC
#undef ENTERFUNC
#endif
#ifdef LEAVEFUNC
#undef LEAVEFUNC
#endif

#ifdef _DEBUG
#define DEBUGMSG(msg)                                 \
  verbose(_T("%*.*s%s\n")                             \
         ,m_callLevel, m_callLevel, EMPTYSTRING       \
         ,msg);

#define ENTERFUNC     {                               \
  verbose(_T("%*.*sEnter %s\n")                       \
         ,m_callLevel, m_callLevel, EMPTYSTRING       \
         ,__TFUNCTION__);                             \
  m_callLevel+= 2;                                    \
}

#define ENTERFUNCPARAM(str) {                         \
  verbose(_T("%*.*sEnter %s(%s)\n")                   \
         ,m_callLevel, m_callLevel, EMPTYSTRING       \
         ,__TFUNCTION__,str.cstr());                  \
  m_callLevel+= 2;                                    \
}

#define LEAVEFUNC     {                               \
  m_callLevel-= 2;                                    \
  verbose(_T("%*.*sLeave %s %s\n")                    \
         ,m_callLevel, m_callLevel,EMPTYSTRING        \
         ,__TFUNCTION__                               \
         ,flagsToString().cstr()                      \
         );                                           \
}

#else
#define DEBUGMSG(msg)
#define ENTERFUNC
#define ENTERFUNCPARAM(str)
#define LEAVEFUNC
#endif

#define INTERRUPTLINE _T("#interrupt#")

ExternEngine::ExternEngine(const String &path) : m_desc(path) {
  m_inputThread       = NULL;
  m_tmpGame           = NULL;
  m_callLevel         = 0;
  clrAllStateFlags();
}

ExternEngine::~ExternEngine() {
  ENTERFUNC;
  quit();
  LEAVEFUNC;
}

void ExternEngine::start() {
  ENTERFUNC;
  __super::start(!Options::getOptions().getShowEngineConsole(), m_desc.getPath(), NULL);
  m_inputThread = new ExternInputThread(getInput());
  sendUCI();
  m_tmpGame = new Game;
  LEAVEFUNC;
}

void ExternEngine::quit() {
  ENTERFUNC;
  stopSearch();
  if(isStarted()) {
    send(_T("quit\n"));
  }
  cleanup();
  LEAVEFUNC;
}

void ExternEngine::killProcess() {
  ENTERFUNC;
  cleanup();
  __super::killProcess();
  LEAVEFUNC;
}

void ExternEngine::cleanup() {
  ENTERFUNC;
  deleteInputThread();
  if(m_tmpGame != NULL) {
    delete m_tmpGame;
    m_tmpGame = NULL;
    DEBUGMSG(_T("tmpGame deleted"));
  }
  m_optionArray.clear();
  clrAllStateFlags();
  LEAVEFUNC;
}

void ExternEngine::deleteInputThread() {
  ENTERFUNC;
  if(m_inputThread == NULL) {
    DEBUGMSG(_T("inputThread already null"));
    goto Return;
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
      __super::killProcess();
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
Return:
  LEAVEFUNC;
}

void ExternEngine::moveNow() {
  ENTERFUNC;
  if(isReady()) {
    send(_T("stop\n"));
  }
  LEAVEFUNC;
}

void ExternEngine::stopSearch() {
  ENTERFUNC;
  if(isReady()) {
    send(_T("stop\n"));
    waitUntilIdle();
  }
  LEAVEFUNC;
}

ExecutableMove ExternEngine::findBestMove(const Game &game, const TimeLimit &timeLimit, bool hint) {
  ENTERFUNC;
  ExecutableMove result;
  if(isIdle()) {
    *m_tmpGame = game;
    sendPosition(game);
    send(_T("go %s\n"), (timeLimit.m_timeout == INFINITE) ? _T("infinite") : format(_T("movetime %d"), timeLimit.m_timeout).cstr());
    setStateFlags(EXE_BUSY);

    for(;;) {
      const String line = getLine();
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext()) {
        const String reply = tok.next();
        if(reply == _T("bestmove")) {
          clrStateFlags(EXE_BUSY);
          result = game.generateMove(tok.next(), MOVE_UCIFORMAT);
          break;
        } else if(reply == INTERRUPTLINE) {
          break;
        }
      }
    }
  }
  LEAVEFUNC;
  return result;
}

void ExternEngine::waitUntilIdle(int timeout) {
  ENTERFUNCPARAM(format(_T("timeout=%d"),timeout));
  const int sleepTime = 100; // msec
  const int count = timeout / sleepTime;
  for(int i = 0; isIdle() && i < count; i++) {
    Sleep(sleepTime);
  }
  if(!isIdle()) {
    killProcess();
  }
  LEAVEFUNC;
}

void ExternEngine::sendUCI() {
  ENTERFUNC;
  m_optionArray.clear();

  String line;
  try {
    String name, author;
    while((line = getLine(2000)) != INTERRUPTLINE) {
      DEBUGMSG(line.cstr());
    }
    send(_T("uci\n"));

    Timestamp startTime;
    while((line = getLine(500)) != _T("uciok")) {
      if(diff(startTime, Timestamp(), TSECOND) > 10) {
        LEAVEFUNC;
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
          DEBUGMSG(format(_T("reply:<%s>"), line.cstr()).cstr());
        }
      }
    }
    sortOptions();
    m_desc.m_name   = name;
    m_desc.m_author = author;
    setStateFlags(EXE_UCIOK);
  } catch(Exception e) {
    DEBUGMSG(format(_T("Exception <%s>. last line read:<%s\n"), e.what(), line.cstr()).cstr());
    LEAVEFUNC;
    throwException(_T("Wrong or no reply on command \"uci\". Doesn't seem to be a UCI chess engine"));
  }
  LEAVEFUNC;
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
  engine.start();
  return engine.getDescription();
}

void ExternEngine::notifyGameChanged(const Game &game) {
  ENTERFUNC;
  if(isReady()) {
    send(_T("ucinewgame\n"));
  }
  LEAVEFUNC;
}

void ExternEngine::setDebug(bool on) {
  ENTERFUNC;
  if(isReady()) {
    send(_T("debug %s\n"), on ? _T("on") : _T("off"));
  }
  LEAVEFUNC;
}

void ExternEngine::sendPosition(const Game &game) const {
  ENTERFUNC;
  if(isIdle()) {
    send(_T("%s"), game.toUCIString().cstr());
  }
  LEAVEFUNC;
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, int value) {
  ENTERFUNC;
  assert(option.getType() == OptionTypeSpin);
  if(isIdle()) {
    send(_T("setoption name %s value %d\n"), option.getName().cstr(), value);
  }
  LEAVEFUNC;
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, bool value) {
  ENTERFUNC;
  assert(option.getType() == OptionTypeCheckbox);
  if(isIdle()) {
    if(&option == &EngineOptionDescription::debugOption) {
      setDebug(value);
    } else {
      send(_T("setoption name %s value %s\n"), option.getName().cstr(), value ? _T("true") : _T("false"));
    }
  }
  LEAVEFUNC;
}

void ExternEngine::setParameterValue(const EngineOptionDescription &option, const String &value) {
  ENTERFUNC;
  assert((option.getType() == OptionTypeCombo) || (option.getType() == OptionTypeString));
  if(isIdle()) {
    send(_T("setoption name %s value %s\n"), option.getName().cstr(), value.cstr());
  }
  LEAVEFUNC;
}

void ExternEngine::clickButton(const EngineOptionDescription &option) {
  ENTERFUNC;
  assert(option.getType() == OptionTypeButton);
  if(isIdle()) {
    send(_T("setoption name %s\n"), option.getName().cstr());
  }
  LEAVEFUNC;
}

void ExternEngine::setParameters(const EngineOptionValueArray &valueArray) {
  ENTERFUNC;
  for(Iterator<EngineOptionValue> it = valueArray.getIterator(); it.hasNext();) {
    setParameterValue(it.next());
  }
  LEAVEFUNC;
}

void ExternEngine::setParameterValue(const EngineOptionValue &v) {
  ENTERFUNC;
  const EngineOptionDescription *optionDesc = m_optionArray.findOptionByName(v.getName());
  if(optionDesc == NULL) {
    Message(_T("Option %s not found for engine %s"), v.getName().cstr(), m_desc.getName().cstr());
    LEAVEFUNC;
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
  LEAVEFUNC;
}

#ifdef _DEBUG
void ExternEngine::send(const TCHAR *format,...) const {
  ENTERFUNC;
  va_list argptr;
  va_start(argptr, format);
  String line = vformat(format, argptr);
  va_end(argptr);
  line.replace('\n',EMPTYSTRING);
  DEBUGMSG(line.cstr());
  __super::send(_T("%s\n"), line.cstr());
  LEAVEFUNC;
}
#endif

String ExternEngine::getLine(int milliseconds) {
  DEFINEMETHODNAME;
  ENTERFUNC;
  const EngineVerboseFields &evf = getOptions().getengineVerboseFields();
  try {
    EngineInfoLine infoLine;
    for(;;) {
      if(!isStarted()) {
        clrStateFlags(EXE_BUSY);
        LEAVEFUNC;
        throwException(_T("%s:Unexpected eof. Extern engine not started"), method);
      }
      String line = m_inputThread->getLine(milliseconds);
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext() && tok.next() == _T("info")) {
        if(isVerbose()) {
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
        LEAVEFUNC;
        return line;
      }
    }
  } catch(Exception e) {
    DEBUGMSG(format(_T("%s:Exception:%s\n"), method, e.what()).cstr());
    LEAVEFUNC;
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

String ExternEngine::flagsToString() const {
  String result;
  const TCHAR *delim = NULL;

#define addStr(s) {                              \
  if(delim) result+=delim; else delim=_T(" ");   \
  result += s;                                   \
}

#define addFlag(f) if(isStateFlagsSet(EXE_##f)) addStr(#f);

  if(isStarted())      addStr(_T("STARTED"));
  if(hasInputThread()) addStr(_T("INPUT"));
  addFlag(UCIOK  );
  addFlag(BUSY   );
  if(isVerbose())      addStr(_T("VERBOSE"));
  return result;
}

String ExternEngine::toString() const {
  String result = flagsToString();
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

#endif // TABLEBASE_BUILDER
