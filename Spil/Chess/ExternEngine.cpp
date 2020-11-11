#include "stdafx.h"
#include <ThreadPool.h>

#if !defined(TABLEBASE_BUILDER)

#include "ExternEngine.h"

//#define _PRINT_DEBUGMSG
//#define _TRACE_ENTERLEAVE

#if defined(ENTERFUNC)
#undef ENTERFUNC
#endif
#if defined(LEAVEFUNC)
#undef LEAVEFUNC
#endif

#if defined(_PRINT_DEBUGMSG)
#define DEBUGMSG(...) debugMsg(__VA_ARGS__)
#else
#define DEBUGMSG(...)
#endif // _PRINT_DEBUGMSG

#if defined(_TRACE_ENTERLEAVE)

#define ENTERFUNCPARAM(...) {                    \
  debugMsg(_T("Enter %s(%s)"),__TFUNCTION__      \
          ,format(__VA_ARGS__).cstr());          \
  m_callLevel += 2;                              \
}

#define ENTERFUNC() ENTERFUNCPARAM(EMPTYSTRING)

#define LEAVEFUNC() {                            \
  m_callLevel -=  2;                             \
  debugMsg(_T("Leave %s"),__TFUNCTION__);        \
}

#else

#define ENTERFUNCPARAM(...)
#define ENTERFUNC()
#define LEAVEFUNC()

#endif // _TRACE_ENTERLEAVE

#define INTERRUPTLINE _T("#interrupt#")

// public
ExternEngine::ExternEngine(Player player, const String &path)
: m_player(player)
, m_desc(path)
, m_inputThread( nullptr)
, m_moveReceiver(nullptr)
, m_stateFlags(0)
, m_threadIsStarted(0)
, m_callLevel(0)
{
  ENTERFUNC();
  clrAllStateFlags();
  LEAVEFUNC();
}

// public
ExternEngine::~ExternEngine() {
  ENTERFUNC();
  quit();
  LEAVEFUNC();
}

class RedirectingInputThread : public InputThread {
protected:
  // the only purpose with this is to redirect verbose-messages to ::verbose
  void vverbose(_In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
    ::verbose(_T("%s.\n"), vformat(format, argptr).cstr());
  }
public:
  RedirectingInputThread(FILE *input) : InputThread(input) {
  }
  String getLine(int timeoutInMilliseconds = INFINITE) override {
    return InputThread::getLine(timeoutInMilliseconds).trim();
  }
};

// public
void ExternEngine::start(AbstractMoveReceiver *mr) {
  ENTERFUNC();
  try {
    __super::start(!Options::getOptions().getShowEngineConsole(), m_desc.getPath(), nullptr);
    m_moveReceiver = mr;
    m_inputThread  = new RedirectingInputThread(getInput()); TRACE_NEW(m_inputThread);
    sendUCI();
    if(m_moveReceiver) {
      ThreadPool::executeNoWait(*this);
      if(!m_threadIsStarted.wait(500)) {
        throwException(_T("Extern enginethread did not start withinh 500 msec."));
      }
    }
  } catch (...) {
    LEAVEFUNC();
    throw;
  }
  LEAVEFUNC();
}

// public
void ExternEngine::quit() {
  ENTERFUNC();
  stopSearch();
  if(isStarted()) {
    send(_T("quit\n"));
  }
  cleanup();
  LEAVEFUNC();
}

// private
void ExternEngine::killProcess() {
  ENTERFUNC();
  cleanup();
  __super::killProcess();
  LEAVEFUNC();
}

// private
void ExternEngine::cleanup() {
  ENTERFUNC();
  if(isThreadRunning()) {
    setStateFlags(EXE_KILLED);
    putInterruptLine();
    while(isThreadRunning()) Sleep(100);
  }
  deleteInputThread();
  m_optionArray.clear();
  clrAllStateFlags();
  LEAVEFUNC();
}

// private
void ExternEngine::deleteInputThread() {
  ENTERFUNC();
  if(!hasInput()) {
    DEBUGMSG(_T("InputThread already null"));
  } else {
    if(isBusy()) {
      putInterruptLine();
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
      DEBUGMSG(_T("Trying to delete inputThread"));
      SAFEDELETE(m_inputThread);
      DEBUGMSG(_T("InputThread deleted"));
    } else {
      verbose(_T("Cannot kill inputthread\n"));
    }
    m_inputThread = nullptr;
    DEBUGMSG(_T("InputThread set to nullptr"));
  }
  LEAVEFUNC();
}

// private
void ExternEngine::waitUntilNotBusy(int timeout) {
  ENTERFUNCPARAM(_T("timeout=%d"), timeout);
  const int sleepTime = 150; // msec
  const int count = timeout / sleepTime;
  for(int i = 0; isBusy() && i < count; i++) {
    Sleep(sleepTime);
  }
  if(isBusy()) {
    const String name = getDescription().getName();
    killProcess();
    m_moveReceiver->putError(_T("Extern engine %s did not reply with \"bestmove\"\n"
                                "before timeout (%d msec). Process is killed")
                            ,name.cstr(), timeout
                            );
  }
  LEAVEFUNC();
}

// private
void ExternEngine::putInterruptLine() { // only called by cleanup and deleteInputThread
  DEBUGMSG(_T("putMessage(INTERRUPTLINE)"));
  m_inputThread->putMessage(INTERRUPTLINE);
}

// private
void ExternEngine::sendUCI() {
  ENTERFUNC();
  m_optionArray.clear();

  String line;
  try {
    String name, author;
    while((line = getLine(2000)) != INTERRUPTLINE) {
      DEBUGMSG(_T("%s"), line.cstr());
    }
    send(_T("uci\n"));

    Timestamp startTime;
    while((line = getLine(500)) != _T("uciok")) {
      if(Timestamp::diff(startTime, Timestamp(), TSECOND) > 10) {
        throwException(_T("timeout"));
      }
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext()) {
        const String command = tok.next();
        if(command == _T("option")) {
          m_optionArray.add(EngineOptionDescription(tok.getRemaining(), (unsigned short)m_optionArray.size()));
          continue;
        } else if(command == _T("id")) {
          const String s = tok.next();
          if(s == _T("name")) {
            name = tok.getRemaining();
            continue;
          } else if(s == _T("author")) {
            author = tok.getRemaining();
            continue;
          }
        }
        DEBUGMSG(_T("Unknown reply:<%s>"), line.cstr());
      }
    }
    sortOptions();
    m_desc.m_name   = name;
    m_desc.m_author = author;
    setStateFlags(EXE_UCIOK);
  } catch(Exception e) {
    DEBUGMSG(_T("Exception <%s>. Last line read:<%s>"), e.what(), line.cstr());
    LEAVEFUNC();
    throwException(_T("Wrong or no reply on command \"uci\". Doesn't seem to be a UCI chess engine"));
  }
  LEAVEFUNC();
}

static int optionsCmp(const EngineOptionDescription &op1, const EngineOptionDescription &op2) {
  if(op1.getType() == op2.getType()) {
    return (int)op1.getIndex() - (int)op2.getIndex();
  } else {
    return op1.getType() - op2.getType();
  }
}

// private
void ExternEngine::sortOptions() {
  m_optionArray.sort(optionsCmp);
}

EngineDescription ExternEngine::getUCIReply(const String &path) { // static
  ExternEngine engine(WHITEPLAYER, path); // player not used
  engine.start();
  return engine.getDescription();
}

// public
void ExternEngine::findBestMove(const Game &game, const TimeLimit &timeLimit) {
  ENTERFUNC();
  if(!isIdle()) {
    DEBUGMSG(_T("engine.isIdle()=false"));
  } else {
    m_game = game;
    sendPosition();
    const int timeout = timeLimit.m_timeout;
    send(_T("go %s\n"), (timeout == INFINITE) ? _T("infinite") : format(_T("movetime %d"), timeout).cstr());
    setStateFlags(EXE_BUSY);
  }
  LEAVEFUNC();
}

// public
void ExternEngine::stopSearch() {
  ENTERFUNC();
  if(isBusy()) {
    send(_T("stop\n"));
    waitUntilNotBusy();
  }
  LEAVEFUNC();
}

// public
void ExternEngine::moveNow() {
  ENTERFUNC();
  stopSearch();
  LEAVEFUNC();
}

// public
void ExternEngine::notifyGameChanged(const Game &game) {
  ENTERFUNC();
  if(isReady()) {
    send(_T("ucinewgame\n"));
  }
  LEAVEFUNC();
}

// private
void ExternEngine::setDebug(bool on) {
  ENTERFUNC();
  if(isReady()) {
    send(_T("debug %s\n"), on ? _T("on") : _T("off"));
  }
  LEAVEFUNC();
}

// private
void ExternEngine::sendPosition() const {
  ENTERFUNC();
  if(isIdle()) {
    send(_T("%s\n"), m_game.toUCIString().cstr());
  }
  LEAVEFUNC();
}

// public
void ExternEngine::setParameterValue(const EngineOptionDescription &option, int value) {
  ENTERFUNC();
  assert(option.getType() == OptionTypeSpin);
  if(isIdle()) {
    send(_T("setoption name %s value %d\n"), option.getName().cstr(), value);
  }
  LEAVEFUNC();
}

// public
void ExternEngine::setParameterValue(const EngineOptionDescription &option, bool value) {
  ENTERFUNC();
  assert(option.getType() == OptionTypeCheckbox);
  if(isIdle()) {
    if(&option == &EngineOptionDescription::debugOption) {
      setDebug(value);
    } else {
      send(_T("setoption name %s value %s\n"), option.getName().cstr(), value ? _T("true") : _T("false"));
    }
  }
  LEAVEFUNC();
}

// public
void ExternEngine::setParameterValue(const EngineOptionDescription &option, const String &value) {
  ENTERFUNC();
  assert((option.getType() == OptionTypeCombo) || (option.getType() == OptionTypeString));
  if(isIdle()) {
    send(_T("setoption name %s value %s\n"), option.getName().cstr(), value.cstr());
  }
  LEAVEFUNC();
}

// public
void ExternEngine::clickButton(const EngineOptionDescription &option) {
  ENTERFUNC();
  assert(option.getType() == OptionTypeButton);
  if(isIdle()) {
    send(_T("setoption name %s\n"), option.getName().cstr());
  }
  LEAVEFUNC();
}

// public
void ExternEngine::setParameters(const EngineOptionValueArray &valueArray) {
  ENTERFUNC();
  for(auto it = valueArray.getIterator(); it.hasNext();) {
    setParameterValue(it.next());
  }
  LEAVEFUNC();
}

// private
void ExternEngine::setParameterValue(const EngineOptionValue &v) {
  ENTERFUNC();
  const EngineOptionDescription *optionDesc = m_optionArray.findOptionByName(v.getName());
  if(optionDesc == nullptr) {
    showWarning(_T("Option %s not found for engine %s"), v.getName().cstr(), m_desc.getName().cstr());
    LEAVEFUNC();
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
  LEAVEFUNC();
}

#if defined(_DEBUG)
void ExternEngine::send(const TCHAR *fmt,...) const {
  ENTERFUNC();
  va_list argptr;
  va_start(argptr, fmt);
  String line = vformat(fmt, argptr);
  va_end(argptr);
  line.replace('\n',EMPTYSTRING);
  DEBUGMSG(_T("%s"), line.cstr());
  __super::send(_T("%s\n"), line.cstr());
  LEAVEFUNC();
}
#endif

UINT ExternEngine::run() {
  DEFINEMETHODNAME;
  ENTERFUNC();
  if(!isReady()) {
    m_moveReceiver->putError(_T("%s:Extern process not started"), method);
    LEAVEFUNC();
    return 0;
  }
  setStateFlags(EXE_THREADRUNNING);
  m_threadIsStarted.notify();
  const EngineVerboseFields &evf = getOptions().getengineVerboseFields();
  try {
    EngineInfoLine infoLine;
    while(!isStateFlagsSet(EXE_KILLED)) {
      const String line = getLine();
      Tokenizer tok(line, _T(" "));
      if(tok.hasNext()) {
        const String reply = tok.next();
        if(reply == _T("info")) {
          if(isVerbose()) {
            infoLine += tok;
            if(infoLine.isReady()) {
              verbose(_T("%s"), infoLine.toString(evf).cstr());
              if(evf.m_pv && (infoLine.m_pv.length() > 0)) {
                String line;
                if(evf.m_depth) line += format(_T("Depth:%2d "), infoLine.m_depth);
                if(evf.m_score) line += format(_T("Score:%6s "), infoLine.m_score.cstr());
                if(evf.m_pv   ) line += getBeautifiedVariant(infoLine.m_pv, evf.m_pvVariantLength);
                updateMessageField(infoLine.m_multiPV, _T("%s"), line.cstr());
              }
              infoLine.reset();
            }
          }
        } else if(reply == _T("bestmove")) {
          const PrintableMove result = m_game.generateMove(tok.next(), MOVE_UCIFORMAT);
          clrStateFlags(EXE_BUSY);
          m_moveReceiver->putMove(result);
        } else if(reply == INTERRUPTLINE) {
          continue;
        }
      }
    }
  } catch(Exception e) {
    m_moveReceiver->putError(_T("%s:Exception:%s"), method, e.what());
  } catch (...) {
    m_moveReceiver->putError(_T("%s:Unknown exception"), method);
  }
  clrStateFlags(EXE_THREADRUNNING);
  LEAVEFUNC();
  return 0;
}

// private
String ExternEngine::getLine(int timeout) {
  DEFINEMETHODNAME;
//  ENTERFUNC();
  try {
    if(!isStarted() || !hasInput()) {
      clrStateFlags(EXE_BUSY);
      throwException(_T("%s:Unexpected eof. Extern engine not started"), method);
    }
//    LEAVEFUNC();
    return m_inputThread->getLine(timeout);
  } catch(Exception e) {
    DEBUGMSG(_T("%s:Exception:%s"), method, e.what());
  }
//  LEAVEFUNC();
  return INTERRUPTLINE;
}

// private
String ExternEngine::getBeautifiedVariant(const String &pv, UINT variantLength) const {
  const int startPly    = m_game.getPlyCount();
  UINT      moveCounter = startPly/2 + 1;
  String    result;
  bool      ok = true;
  if(m_game.getPlayerInTurn() == BLACKPLAYER) {
    result = format(_T("%u. -"), moveCounter);
  }
  try {
    const UINT lastMove = moveCounter + variantLength;
    for(Tokenizer tok(pv, _T(" ")); tok.hasNext() && ok;) {
      const PrintableMove m = m_game.generateMove(tok.next(), MOVE_UCIFORMAT);
      if(m_game.getPlayerInTurn() == WHITEPLAYER) {
        result += format(_T("%u. %s"), moveCounter, m.toString().cstr());
      } else {
        result += format(_T(", %s "), m.toString().cstr());
        if(++moveCounter >= lastMove) break;
      }
      m_game.doMove(m_game.generateMove(m));
    }
  } catch(Exception e) {
    result = e.what();
    ok = false;
  }
  while(m_game.getPlyCount() > startPly) {
    m_game.undoMove();
  }
  if(!ok) {
    m_moveReceiver->putError(_T("%s"), result.cstr());
    return format(_T("Invalid reply from %s:%s")
                 ,getDescription().getName().cstr()
                 ,pv.cstr());
  }
  return result;
}

// public
String ExternEngine::flagsToString() const {
  String result;
  const TCHAR *delim = nullptr;

#define addStr(s) {                              \
  if(delim) result+=delim; else delim=_T(" ");   \
  result += s;                                   \
}

#define addFlag(f) if(isStateFlagsSet(EXE_##f)) addStr(#f);

  if(isStarted())      addStr(_T("STARTED"));
  if(hasInput())       addStr(_T("HASINPUT"));
  addFlag(UCIOK        );
  addFlag(BUSY         );
  addFlag(THREADRUNNING);
  addFlag(KILLED       );
  if(isVerbose())      addStr(_T("VERBOSE"));
  return result;
}

// public
String ExternEngine::toString() const {
  String result = format(_T("Flags  :%s"), flagsToString().cstr());
  if(isStarted()) {
    if(m_optionArray.size() > 0) {
      result += _T("\nOptions:\n");
      result += indentString(m_optionArray.toString(), 2);
    }
  }
  return result;
}

// private
void ExternEngine::debugMsg(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  verbose(_T("%s:%*.*s%s (%s)\n")
         ,getPlayerShortNameEnglish(m_player)
         ,m_callLevel,m_callLevel, EMPTYSTRING
         ,msg.cstr()
         ,flagsToString().cstr()
         );
}

#endif // TABLEBASE_BUILDER
