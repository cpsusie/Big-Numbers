#include "stdafx.h"
#include "TraceDlgThread.h"
#include "MoveFinderThread.h"
#include "MoveFinderRandom.h"
#include "MoveFinderEndGame.h"
#include "MoveFinderExternEngine.h"
#include "MoveFinderRemotePlayer.h"

#ifdef ENTERFUNC
#undef ENTERFUNC
#endif
#ifdef LEAVEFUNC
#undef LEAVEFUNC
#endif

#define STATESTR()  getStateName(getState())
#define PLAYERSTR() ((m_player==WHITEPLAYER)?_T("W"):_T("B"))

#define _PRINT_DEBUGMSG
#define _DEBUG_CHECKSTATE
#define _TRACE_ENTERLEAVE

#ifdef _PRINT_DEBUGMSG
#define DEBUGMSG(...) debugMsg(__VA_ARGS__)
#else
#define DEBUGMSG(...)
#endif

#ifdef _DEBUG_CHECKSTATE
#define CHECKSTATE(s1,...) checkState(__TFUNCTION__, __LINE__, s1, __VA_ARGS__ ,-1)
#else 
#define CHECKSTATE(s1,...)
#endif // _DEBUG_CHECKSTATES

#ifdef _TRACE_ENTERLEAVE

#define ENTERFUNCPARAM(...) {                    \
  debugMsg(_T("Enter %s(%s)"), __TFUNCTION__     \
          ,format(__VA_ARGS__).cstr());          \
  m_callLevel+= 2;                               \
}

#define ENTERFUNC() ENTERFUNCPARAM(_T(""))

#define LEAVEFUNC() {                            \
  m_callLevel-= 2;                               \
  debugMsg(_T("Leave %s"),__TFUNCTION__);        \
}

#else

#define ENTERFUNCPARAM(...)
#define ENTERFUNC()
#define LEAVEFUNC()

#endif // _TRACE_ENTERLEAVE

OpeningLibrary MoveFinderThread::s_openingLibrary;

// public
MoveFinderThread::MoveFinderThread(Player player) : m_player(player) {
  setDeamon(true);
  m_state      = MFTS_IDLE;
  m_callLevel  = 0;
  m_moveFinder = NULL;
}

// public
MoveFinderThread::~MoveFinderThread() {
  ENTERFUNC();
  putRequest(REQUEST_RESET);
  putRequest(REQUEST_KILL);
  while(stillActive()) {
    Sleep(20);
  }
  LEAVEFUNC();
}

// public
void MoveFinderThread::resetMoveFinder() {
  ENTERFUNC();
  putRequest(REQUEST_RESET);
  LEAVEFUNC();
}

// public
void MoveFinderThread::startThinking(const Game &game, const TimeLimit &timeLimit, bool hint) {
  ENTERFUNC();
  const bool talking = getOptions().getTraceWindowVisible();
  if(talking) {
    clearVerbose();
  }
  const GameResult gr = game.findGameResult();
  if(isBusy()) {
    DEBUGMSG(_T("StartThinking called while already busy"));
    putRequest(REQUEST_RESET);
  } else if(gr != NORESULT) {
    putRequest(REQUEST_NULLMOVE);
  } else {
    m_gate.wait();
    m_searchResult.m_move.setNoMove();
    putRequest(MoveFinderThreadRequest(game, timeLimit, hint));
    m_gate.signal();
  }
  LEAVEFUNC();
}

// public
void MoveFinderThread::stopSearch() {
  ENTERFUNC();
  m_gate.wait();
  if(getState() == MFTS_BUSY) {
    setState(MFTS_STOPPENDING);
    putRequest(REQUEST_STOPSEARCH);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
void MoveFinderThread::moveNow() {
  ENTERFUNC();
  m_gate.wait();
  if(getState() == MFTS_BUSY) {
    putRequest(REQUEST_MOVENOW);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
SearchMoveResult MoveFinderThread::getSearchResult() const {
  ENTERFUNC();
  CHECKSTATE(MFTS_MOVEREADY);
  LEAVEFUNC();
  return m_searchResult;
}

// public
void MoveFinderThread::notifyGameChanged(const Game &game) {
  ENTERFUNC();
  putRequest(MoveFinderThreadRequest(game));
  LEAVEFUNC();
}

// public
bool MoveFinderThread::notifyMove(const MoveBase &move) {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE,MFTS_MOVEREADY);
  bool result;
  try {
    if(m_moveFinder) {
      m_moveFinder->notifyMove(move);
    }
    setState(MFTS_IDLE);
    result = true;
  } catch(TcpException e) {
    handleTcpException(e);
    result = false;
  }
  m_gate.signal();
  LEAVEFUNC();
  return result;
}

// public
bool MoveFinderThread::acceptUndoMove() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE,MFTS_PREPARESEARCH,MFTS_BUSY,MFTS_MOVEREADY);
  bool result;
  try {
    result = m_moveFinder ? m_moveFinder->acceptUndoMove() : true;
  } catch(TcpException e) {
    handleTcpException(e);
    result = false;
  }
  m_gate.signal();
  LEAVEFUNC();
  return result;
}

// Only called from outside
void MoveFinderThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  ENTERFUNC();
  if(id == TRACEWINDOW_ACTIVE) {
    m_gate.wait();
    if(m_moveFinder != NULL) {
      const bool verbose = *(const bool*)newValue;
      m_moveFinder->setVerbose(verbose);
    }
    m_gate.signal();
  }
  LEAVEFUNC();
}

// mainloop
UINT MoveFinderThread::run() {
  ENTERFUNC();
  setSelectedLanguageForThread();

  while(getState() != MFTS_KILLED) {
    DEBUGMSG(_T("Waiting for request"));

    const MoveFinderThreadRequest request = m_inputQueue.get();

    DEBUGMSG(_T("Got request %s"), request.toString().cstr());

    switch(request.getType()) {
    case REQUEST_FINDMOVE  :
      try {
        handleFindMoveRequest(request.getFindMoveParam());
      } catch(TcpException e) {
        m_errorMessage = e.what();
        setState(MFTS_ERROR);
        disconnect();
      } catch(Exception e) {
        m_errorMessage = e.what();
        setState(MFTS_ERROR);
        putRequest(REQUEST_RESET);
      } catch(CSimpleException *e) {
        TCHAR msg[1024];
        e->GetErrorMessage(msg, sizeof(msg));
        e->Delete();
        m_errorMessage = msg;
        setState(MFTS_ERROR);
        putRequest(REQUEST_RESET);
      } catch(...) {
        m_errorMessage = format(_T("Unknown Exception (State=%s)"), STATESTR());
        setState(MFTS_ERROR);
        putRequest(REQUEST_RESET);
      }
      break;

    case REQUEST_NULLMOVE  :
      handleNullMoveRequest();
      break;

    case REQUEST_STOPSEARCH:
      handleStopSearchRequest();
      break;

    case REQUEST_MOVENOW   :
      handleMoveNowRequest();
      break;

    case REQUEST_FETCHMOVE :
      handleFetchMoveRequest(request.getSearchResult());
      break;

    case REQUEST_GAMECHANGED:
      handleGameChangedRequest(request.getGameChangedParam().getGame());
      break;

    case REQUEST_RESET     :
      handleResetRequest();
      break;

    case REQUEST_DISCONNECT:
      handleDisconnectRequest();
      break;

    case REQUEST_KILL      :
      handleKillRequest();
      break;

    default                :
      DEBUGMSG(_T("Invalid request:%s"), request.toString().cstr());
      break;
    }
  }
  LEAVEFUNC();
  return 0;
}

// private
void MoveFinderThread::handleFindMoveRequest(FindMoveRequestParam param) {
  ENTERFUNC();

  m_gate.wait();
  CHECKSTATE(MFTS_IDLE);
  setState(MFTS_PREPARESEARCH);
  m_gate.signal();

  const bool talking = getOptions().getTraceWindowVisible();

  if(getOptions().isOpeningLibraryEnabled() && !isRemote()) {
    const PrintableMove libMove = getOpeningLibrary().findLibraryMove(param.getGame(), talking);
    if(libMove.isMove()) {
      setMoveFinder(NULL);
      setState(MFTS_BUSY);
      putRequest(MoveFinderThreadRequest(libMove, param.isHint()));
      LEAVEFUNC();
      return;
    }
  }

  if(isNewMoveFinderNeeded(param)) {
    allocateMoveFinder(param);
  }

  m_gate.wait();
  if(m_moveFinder != NULL) {
    if(getState() == MFTS_PREPARESEARCH) {
      setState(MFTS_BUSY);
      m_moveFinder->findBestMove(param, talking);
    } else {
      // do nothing!! User has pressed undo, so state is idle
    }
  } else {
    setState(MFTS_IDLE);
    m_gate.signal();
    LEAVEFUNC();
    throwException(_T("No moveFinder allocated"));
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleNullMoveRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE);
  m_searchResult.clear();
  setState(MFTS_MOVEREADY);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleStopSearchRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_STOPPENDING);
  if(m_moveFinder) {
    m_moveFinder->stopSearch();
  }
  setState(MFTS_IDLE);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleMoveNowRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE, MFTS_BUSY);
  if((getState() == MFTS_BUSY) && m_moveFinder) {
    m_moveFinder->moveNow();
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleFetchMoveRequest(const SearchMoveResult &searchResult) {
  ENTERFUNC();
  m_gate.wait();
  switch(getState()) {
  case MFTS_IDLE       :
    break;
  case MFTS_STOPPENDING:
    setState(MFTS_IDLE);
    break;
  case MFTS_BUSY       :
    m_searchResult = searchResult;
    setState(m_searchResult.isMove() ? MFTS_MOVEREADY : MFTS_IDLE);
    break;
  default              :
    CHECKSTATE(MFTS_IDLE, MFTS_BUSY, MFTS_STOPPENDING);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleGameChangedRequest(const Game &game) {
  ENTERFUNC();
  CHECKSTATE(MFTS_IDLE,MFTS_PREPARESEARCH,MFTS_BUSY,MFTS_STOPPENDING,MFTS_MOVEREADY);
  if(isBusy()) {
    stopSearch();
    DEBUGMSG(_T("While(isBusy())"));
    for(int count = 0; isBusy() && count < 10; count++) {
      Sleep(100);
    }
    DEBUGMSG(_T("End while"));
    if(isBusy()) {
      DEBUGMSG(_T("Still busy. putRequest(REQUEST_RESET)"));
      putRequest(REQUEST_RESET);
      LEAVEFUNC();
      return;
    }
  }

  m_gate.wait();

  try {
    if(m_moveFinder) {
      m_moveFinder->notifyGameChanged(game);
    }
    setState(MFTS_IDLE);
  } catch(TcpException e) {
    handleTcpException(e);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleResetRequest() {
  ENTERFUNC();
  setMoveFinder(NULL);
  setState(MFTS_IDLE);
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleDisconnectRequest() {
  ENTERFUNC();
  setRemote(Game(), SocketChannel());
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleKillRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE);
  setState(MFTS_KILLED);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void MoveFinderThread::handleTcpException(const TcpException &e) {
  ENTERFUNC();
  m_errorMessage = e.what();
  disconnect();
  setState(MFTS_ERROR);
  putRequest(REQUEST_RESET);
  LEAVEFUNC();
}

// private
const OpeningLibrary &MoveFinderThread::getOpeningLibrary() { // static
  if(!s_openingLibrary.isLoaded()) {
    s_openingLibrary.load(IDR_OPENINGLIBRARY);
  }
  return s_openingLibrary;
}

// private
bool MoveFinderThread::isNewMoveFinderNeeded(const FindMoveRequestParam &param) const {
  if(m_moveFinder == NULL) {
    return true;
  }
  if(isRemote()) {
    return !m_moveFinder->isRemote();
  }

  const PositionType gamePositionType = param.getGame().getPositionType();
  switch(gamePositionType) {
  case NORMAL_POSITION      :
    return !isRightNormalPlayMoveFinder(param);

  case DRAW_POSITION     :
    return m_moveFinder->getPositionType() != NORMAL_POSITION;

  case TABLEBASE_POSITION    :
    { EndGameTablebase *tablebase = findMatchingTablebase(param.getGame());
      if(tablebase == NULL) {
        return !isRightNormalPlayMoveFinder(param);
      } else {
        return !isRightTablebaseMoveFinder(tablebase);
      }
      return false;
    }

  default               :
    throwException(_T("%s:Unknown positiontype:%d"), __TFUNCTION__, gamePositionType);
    return false;
  }
}

// private
bool MoveFinderThread::isRightNormalPlayMoveFinder(const FindMoveRequestParam &param) const {
  if(m_moveFinder->getPositionType() != NORMAL_POSITION) {
    return false;
  }

#ifndef TABLEBASE_BUILDER
  if(param.getTimeLimit().m_timeout == 0) {
    return m_moveFinder->getEngineType() == RANDOM_ENGINE;
  }

  return m_moveFinder->getEngineType() == EXTERN_ENGINE;
#else
  return m_moveFinder->getEngineType() == RANDOM_ENGINE;
#endif
}

// private
bool MoveFinderThread::isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const {
  if(m_moveFinder->getPositionType() != TABLEBASE_POSITION) {
    return false;
  }
  bool swapPlayers;
  return ((MoveFinderEndGame*)m_moveFinder)->getPositionSignature().match(tablebase->getKeyDefinition().getPositionSignature(), swapPlayers);
}

// private
EndGameTablebase *MoveFinderThread::findMatchingTablebase(const Game &g) const {
  if(!getOptions().isEndGameTablebaseEnabled()) {
    return NULL;
  }
  bool swapPlayers;
  EndGameTablebase *db = EndGameTablebase::getInstanceBySignature(g.getPositionSignature(), swapPlayers);
  return (db && (db->exist(DECOMPRESSEDTABLEBASE) || db->exist(COMPRESSEDTABLEBASE))) ? db : NULL;
}

// private
void MoveFinderThread::allocateMoveFinder(const FindMoveRequestParam &param) {
  if(isRemote()) {
    setMoveFinder(new MoveFinderRemotePlayer(getPlayer(), m_inputQueue, m_channel));
    return;
  }
  switch(param.getGame().getPositionType()) {
  case NORMAL_POSITION  :
  case DRAW_POSITION    :
    setMoveFinder(newMoveFinderNormalPlay(param));
    break;

  case TABLEBASE_POSITION:
    { EndGameTablebase *tablebase = findMatchingTablebase(param.getGame());
      if(tablebase == NULL) {
        setMoveFinder(newMoveFinderNormalPlay(param));
      } else {
        setMoveFinder(new MoveFinderEndGame(getPlayer(), m_inputQueue, tablebase));
      }
    }
    break;
  }
}

// private
AbstractMoveFinder *MoveFinderThread::newMoveFinderNormalPlay(const FindMoveRequestParam &param) {
#ifndef TABLEBASE_BUILDER
  if(param.getTimeLimit().m_timeout == 0) {
    return new MoveFinderRandomPlay(getPlayer(), m_inputQueue);
  } else {
    return new MoveFinderExternEngine(getPlayer(), m_inputQueue);
  }
#else
    return new MoveFinderRandomPlay(getPlayer(), m_inputQueue);
#endif
}

class StateStringifier : public AbstractStringifier<UINT> {
public:
  String toString(const UINT &state) {
    return MoveFinderThread::getStateName((MoveFinderThreadState)state);
  }
};

class StateSet : public BitSet32 {
public:
  String toString() const {
    return BitSet32::toString(&StateStringifier());
  }
};

// private
void MoveFinderThread::checkState(const TCHAR *method, int line, MoveFinderThreadState s1,...) const {
  StateSet expectedStates;
  va_list argptr;
  va_start(argptr,s1);
  expectedStates.add(s1);
  MoveFinderThreadState s;
  while((int)(s = va_arg(argptr,MoveFinderThreadState)) >= 0) {
    expectedStates.add(s);
  }
  if (!expectedStates.contains(m_state)) {
    const String errMsg = format(_T("Invalid state in %s(%s) line %d. m_state=%s. Expected={%s}")
                                ,method, getPlayerNameEnglish(getPlayer()), line
                                ,getStateName(m_state)
                                ,expectedStates.toString().cstr()
                                );
    verbose(_T("%s\n"), errMsg.cstr());
    throwException(errMsg);
  }
}

// public
String MoveFinderThread::getName() const {
  m_gate.wait();
  const String result = m_moveFinder ? m_moveFinder->getName() : _T("None");
  m_gate.signal();
  return result;
}

//private
void MoveFinderThread::setState(MoveFinderThreadState newState) {
  if(newState != m_state) {
    DEBUGMSG(_T("Stateshift:%s->%s"), STATESTR(), getStateName(newState));
    setProperty(MFTP_STATE, m_state, newState);
  }
}

// private
void MoveFinderThread::setMoveFinder(AbstractMoveFinder *moveFinder) {
  ENTERFUNC();
  m_gate.wait();
  if(moveFinder != m_moveFinder) {

    DEBUGMSG(_T("Change movefinder(%s->%s)")
            ,m_moveFinder?m_moveFinder->getName().cstr():_T("NULL")
            ,moveFinder  ?  moveFinder->getName().cstr():_T("NULL"));

    const AbstractMoveFinder *old = m_moveFinder;
    setProperty(MFTP_MOVEFINDER, m_moveFinder, moveFinder);

    if(old != NULL) {
      delete old;
    }
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
void MoveFinderThread::setRemote(const Game &game, const SocketChannel &channel) {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(MFTS_IDLE);

  const bool oldRemote = isRemote();
  m_channel = channel;
  const bool newRemote = isRemote();
  if(newRemote != oldRemote) {

    notifyPropertyChanged(MFTP_REMOTE, &oldRemote, &newRemote);

    FindMoveRequestParam param(game, TimeLimit(), false);
    if(isNewMoveFinderNeeded(param)) {
      allocateMoveFinder(param);
    }
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
void MoveFinderThread::disconnect() {
  ENTERFUNC();
  putRequest(REQUEST_DISCONNECT);
  LEAVEFUNC();
}

// public
void MoveFinderThread::printState(Player computerPlayer, bool detailed) {
  String result;
  m_gate.wait();
  result = format(_T("MoveFinderThread\n"
                     "   State           : %s\n"
                     "   Remote          : %s\n")
                 ,STATESTR()
                 ,boolToStr(isRemote())
                 );
  if(m_moveFinder) {
    result += format(_T("   Movefinder      : %s\n"), m_moveFinder->getName().cstr());
    result += format(_T("     state:\n%s"), m_moveFinder->getStateString(computerPlayer, detailed).cstr());
  } else {
    result += _T("   No movefinder allocated\n");
  }

  m_gate.signal();
  verbose(_T("%s\n"), result.cstr());
}

// public
const TCHAR *MoveFinderThread::getStateName(MoveFinderThreadState state) { // static 
#define caseStr(s) case MFTS_##s: return _T(#s);
  switch(state) {
  caseStr(IDLE         )
  caseStr(PREPARESEARCH)
  caseStr(BUSY         )
  caseStr(STOPPENDING  )
  caseStr(MOVEREADY    )
  caseStr(ERROR        )
  caseStr(KILLED       )
  default:return _T("??");
  }
#undef caseStr
}

// private
void MoveFinderThread::debugMsg(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  verbose(_T("%s:%*.*s%s (state=%s)\n")
         ,PLAYERSTR()
         ,m_callLevel,m_callLevel, EMPTYSTRING
         ,msg.cstr()
         ,STATESTR()
         );
}
