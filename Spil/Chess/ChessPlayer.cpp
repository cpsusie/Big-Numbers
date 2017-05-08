#include "stdafx.h"
#include "TraceDlgThread.h"
#include "ChessPlayer.h"
#include "MoveFinderRandom.h"
#include "MoveFinderEndGame.h"
#include "MoveFinderExternEngine.h"
#include "MoveFinderRemotePlayer.h"

#define _PRINT_DEBUGMSG
//#define _TRACE_ENTERLEAVE
#define _DEBUG_CHECKSTATE

#ifdef ENTERFUNC
#undef ENTERFUNC
#endif
#ifdef LEAVEFUNC
#undef LEAVEFUNC
#endif

#define STATESTR()  getStateName(getState())

#ifdef _PRINT_DEBUGMSG
#define DEBUGMSG(...) debugMsg(__VA_ARGS__)
#else
#define DEBUGMSG(...)
#endif // _PRINT_DEBUGMSG

#ifdef _DEBUG_CHECKSTATE
#define CHECKSTATE(s1,...) checkState(__TFUNCTION__, __LINE__, s1, __VA_ARGS__ ,-1)
#else 
#define CHECKSTATE(s1,...)
#endif // _DEBUG_CHECKSTATE

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

OpeningLibrary ChessPlayer::s_openingLibrary;

// public
ChessPlayer::ChessPlayer(Player player) : m_player(player) {
  setDeamon(true);
  m_state      = CPS_IDLE;
  m_callLevel  = 0;
  m_moveFinder = NULL;
}

// public
ChessPlayer::~ChessPlayer() {
  ENTERFUNC();
  putRequest(REQUEST_RESET);
  putRequest(REQUEST_KILL);
  while(stillActive()) {
    Sleep(20);
  }
  LEAVEFUNC();
}

// public
void ChessPlayer::resetMoveFinder() {
  ENTERFUNC();
  putRequest(REQUEST_RESET);
  LEAVEFUNC();
}

// public
void ChessPlayer::startSearch(const Game &game, const TimeLimit &timeLimit, bool hint) {
  ENTERFUNC();
  const bool talking = getOptions().getTraceWindowVisible();
  if(talking) {
    clearVerbose();
  }
  const GameResult gr = game.findGameResult();
  if(isBusy()) {
    putRequest(ChessPlayerRequest(format(_T("%s called while already busy"), __TFUNCTION__),true));
  } else if(gr != NORESULT) {
    putRequest(REQUEST_NULLMOVE);
  } else {
    m_gate.wait();
    m_searchResult.m_move.setNoMove();
    putRequest(ChessPlayerRequest(game, timeLimit, hint, talking));
    m_gate.signal();
  }
  LEAVEFUNC();
}

// public
void ChessPlayer::stopSearch() {
  ENTERFUNC();
  m_gate.wait();
  if(getState() == CPS_BUSY) {
    setState(CPS_STOPPENDING);
    putRequest(REQUEST_STOPSEARCH);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
void ChessPlayer::moveNow() {
  ENTERFUNC();
  m_gate.wait();
  if(getState() == CPS_BUSY) {
    putRequest(REQUEST_MOVENOW);
  }
  m_gate.signal();
  LEAVEFUNC();
}

// public
SearchMoveResult ChessPlayer::getSearchResult() const {
  ENTERFUNC();
  CHECKSTATE(CPS_MOVEREADY);
  LEAVEFUNC();
  return m_searchResult;
}

// public
void ChessPlayer::notifyGameChanged(const Game &game) {
  ENTERFUNC();
  putRequest(ChessPlayerRequest(game));
  LEAVEFUNC();
}

// public
bool ChessPlayer::notifyMove(const MoveBase &move) {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_IDLE,CPS_MOVEREADY);
  bool result;
  try {
    if(m_moveFinder) {
      m_moveFinder->notifyMove(move);
    }
    setState(CPS_IDLE);
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
bool ChessPlayer::acceptUndoMove() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_IDLE,CPS_PREPARESEARCH,CPS_BUSY,CPS_MOVEREADY);
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
void ChessPlayer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
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
UINT ChessPlayer::run() {
  ENTERFUNC();
  setSelectedLanguageForThread();

  while(getState() != CPS_KILLED) {
    DEBUGMSG(_T("Waiting for request"));

    const ChessPlayerRequest request = m_inputQueue.get();

    DEBUGMSG(_T("Got request %s"), request.toString().cstr());

    switch(request.getType()) {
    case REQUEST_FINDMOVE  :
      try {
        handleFindMoveRequest(request.getFindMoveParam());
      } catch(TcpException e) {
        handleTcpException(e);
      } catch(Exception e) {
        putRequest(ChessPlayerRequest(e.what(), true));
      } catch(CSimpleException *e) {
        TCHAR msg[1024];
        e->GetErrorMessage(msg, sizeof(msg));
        e->Delete();
        putRequest(ChessPlayerRequest(msg, true));
      } catch(...) {
        putRequest(ChessPlayerRequest(format(_T("Unknown Exception (State=%s)"), STATESTR()), true));
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
      handleFetchMoveRequest(request.getFetchMoveParam());
      break;

    case REQUEST_GAMECHANGED:
      handleGameChangedRequest(request.getGameChangedParam());
      break;

    case REQUEST_SHOWMESSAGE:
      handleShowMessageRequest(request.getShowMessageParam());
      break;

    case REQUEST_RESET     :
      handleResetRequest();
      break;

    case REQUEST_CONNECT   :
      handleConnectRequest(request.getConnectParam());
      break;

    case REQUEST_DISCONNECT:
      handleDisconnectRequest();
      break;

    case REQUEST_KILL      :
      handleKillRequest();
      break;

    default                :
      debugMsg(_T("Invalid request:%s"), request.toString().cstr());
      break;
    }
  }
  LEAVEFUNC();
  return 0;
}

// private
void ChessPlayer::handleFindMoveRequest(const FindMoveRequestParam &param) {
  ENTERFUNC();

  m_gate.wait();
  CHECKSTATE(CPS_IDLE,CPS_MOVEREADY);
  setState(CPS_PREPARESEARCH);
  m_gate.signal();

  if(getOptions().isOpeningLibraryEnabled() && !isRemote()) {
    const PrintableMove libMove = getOpeningLibrary().findLibraryMove(param.getGame(), param.isVerbose());
    if(libMove.isMove()) {
      setMoveFinder(NULL);
      setState(CPS_BUSY);
      putRequest(ChessPlayerRequest(libMove, param.isHint()));
      LEAVEFUNC();
      return;
    }
  }

  if(isNewMoveFinderNeeded(param)) {
    allocateMoveFinder(param);
  }

  m_gate.wait();
  if(m_moveFinder != NULL) {
    if(getState() == CPS_PREPARESEARCH) {
      setState(CPS_BUSY);
      m_moveFinder->findBestMove(param);
    } else {
      // do nothing!! User has pressed undo, so state is idle
    }
  } else {
    setState(CPS_IDLE);
    m_gate.signal();
    LEAVEFUNC();
    throwException(_T("No moveFinder allocated"));
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleNullMoveRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_IDLE);
  m_searchResult.clear();
  setState(CPS_MOVEREADY);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleStopSearchRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_STOPPENDING);
  if(m_moveFinder) {
    m_moveFinder->stopSearch();
  }
  setState(CPS_IDLE);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleMoveNowRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_IDLE, CPS_BUSY);
  if((getState() == CPS_BUSY) && m_moveFinder) {
    m_moveFinder->moveNow();
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleFetchMoveRequest(const FetchMoveRequestParam &param) {
  ENTERFUNC();
  m_gate.wait();
  switch(getState()) {
  case CPS_IDLE       :
    break;
  case CPS_STOPPENDING:
    setState(CPS_IDLE);
    break;
  case CPS_BUSY       :
    m_searchResult = param;
    setState(m_searchResult.isMove() ? CPS_MOVEREADY : CPS_IDLE);
    break;
  default              :
    CHECKSTATE(CPS_IDLE, CPS_BUSY, CPS_STOPPENDING);
    break;
  }
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleGameChangedRequest(const GameChangedRequestParam &param) {
  ENTERFUNC();
  CHECKSTATE(CPS_IDLE,CPS_PREPARESEARCH,CPS_BUSY,CPS_STOPPENDING,CPS_MOVEREADY);
  if(isBusy()) {
    stopSearch();
    DEBUGMSG(_T("While(isBusy())"));
    for(int count = 0; isBusy() && count < 10; count++) {
      Sleep(100);
    }
    DEBUGMSG(_T("End while"));
    if(isBusy()) {
      putRequest(ChessPlayerRequest(_T("Still busy"),true));
      LEAVEFUNC();
      return;
    }
  }

  m_gate.wait();

  try {
    if(m_moveFinder) {
      m_moveFinder->notifyGameChanged(param.getGame());
    }
    setState(CPS_IDLE);
  } catch(TcpException e) {
    handleTcpException(e);
  }
  m_gate.signal();
  LEAVEFUNC();
}

void ChessPlayer::handleShowMessageRequest(const ShowMessageRequestParam &param) {
  setProperty(CPP_MESSAGETEXT, m_messageText, param.getMessage());
  if (param.isError()) {
    putRequest(REQUEST_RESET);
  }
}

// private
void ChessPlayer::handleResetRequest() {
  ENTERFUNC();
  setMoveFinder(NULL);
  setProperty(CPP_MESSAGETEXT, m_messageText, _T(""));
  setState(CPS_IDLE);
  LEAVEFUNC();
}

// private
void ChessPlayer::handleConnectRequest(const ConnectRequestParam &param) {
  ENTERFUNC();
  setRemote(param.getChannel());
  LEAVEFUNC();
}

// private
void ChessPlayer::handleDisconnectRequest() {
  ENTERFUNC();
  setRemote(SocketChannel());
  LEAVEFUNC();
}

// private
void ChessPlayer::setRemote(const SocketChannel &channel) {
  ENTERFUNC();
  CHECKSTATE(CPS_IDLE);

  const bool oldRemote = isRemote();
  m_channel = channel;
  const bool newRemote = isRemote();
  if(newRemote != oldRemote) {
    if(newRemote) {
      allocateRemoteMoveFinder();
    } else {
      putRequest(REQUEST_RESET);
    }
    notifyPropertyChanged(CPP_REMOTE, &oldRemote, &newRemote);
  }
  LEAVEFUNC();
}

// private
void ChessPlayer::handleKillRequest() {
  ENTERFUNC();
  m_gate.wait();
  CHECKSTATE(CPS_IDLE);
  setState(CPS_KILLED);
  m_gate.signal();
  LEAVEFUNC();
}

// private
void ChessPlayer::handleTcpException(const TcpException &e) {
  ENTERFUNC();
  disconnect();
  putRequest(ChessPlayerRequest(e.what(), true));
  LEAVEFUNC();
}

// private
const OpeningLibrary &ChessPlayer::getOpeningLibrary() { // static
  if(!s_openingLibrary.isLoaded()) {
    s_openingLibrary.load(IDR_OPENINGLIBRARY);
  }
  return s_openingLibrary;
}

// private
bool ChessPlayer::isNewMoveFinderNeeded(const FindMoveRequestParam &param) const {
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
bool ChessPlayer::isRightNormalPlayMoveFinder(const FindMoveRequestParam &param) const {
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
bool ChessPlayer::isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const {
  if(m_moveFinder->getPositionType() != TABLEBASE_POSITION) {
    return false;
  }
  bool swapPlayers;
  return ((MoveFinderEndGame*)m_moveFinder)->getPositionSignature().match(tablebase->getKeyDefinition().getPositionSignature(), swapPlayers);
}

// private
EndGameTablebase *ChessPlayer::findMatchingTablebase(const Game &g) const {
  if(!getOptions().isEndGameTablebaseEnabled()) {
    return NULL;
  }
  bool swapPlayers;
  EndGameTablebase *db = EndGameTablebase::getInstanceBySignature(g.getPositionSignature(), swapPlayers);
  return (db && (db->exist(DECOMPRESSEDTABLEBASE) || db->exist(COMPRESSEDTABLEBASE))) ? db : NULL;
}

void ChessPlayer::allocateRemoteMoveFinder() {
  assert(isRemote());
  setMoveFinder(new MoveFinderRemotePlayer(getPlayer(), m_inputQueue, m_channel));
}

// private
void ChessPlayer::allocateMoveFinder(const FindMoveRequestParam &param) {
  assert(!isRemote());
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
AbstractMoveFinder *ChessPlayer::newMoveFinderNormalPlay(const FindMoveRequestParam &param) {
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
    return ChessPlayer::getStateName((ChessPlayerState)state);
  }
};

class StateSet : public BitSet32 {
public:
  String toString() const {
    return BitSet32::toString(&StateStringifier());
  }
};

// private
void ChessPlayer::checkState(const TCHAR *method, int line, ChessPlayerState s1,...) const {
  StateSet expectedStates;
  va_list argptr;
  va_start(argptr,s1);
  expectedStates.add(s1);
  ChessPlayerState s;
  while((int)(s = va_arg(argptr,ChessPlayerState)) >= 0) {
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
String ChessPlayer::getName() const {
  m_gate.wait();
  const String result = m_moveFinder ? m_moveFinder->getName() : _T("None");
  m_gate.signal();
  return result;
}

//private
void ChessPlayer::setState(ChessPlayerState newState) {
  if(newState != m_state) {
    if(m_state == CPS_KILLED) {
      DEBUGMSG(_T("State already set to killed. No more transitions allowed"));
      return;
    } else {
      DEBUGMSG(_T("Stateshift:%s->%s"), STATESTR(), getStateName(newState));
      setProperty(CPP_STATE, m_state, newState);
    }
  }
}

// private
void ChessPlayer::setMoveFinder(AbstractMoveFinder *moveFinder) {
  ENTERFUNC();
  m_gate.wait();
  if(moveFinder != m_moveFinder) {

    DEBUGMSG(_T("Change movefinder(%s->%s)")
            ,m_moveFinder?m_moveFinder->getName().cstr():_T("NULL")
            ,moveFinder  ?  moveFinder->getName().cstr():_T("NULL"));

    const AbstractMoveFinder *old = m_moveFinder;
    setProperty(CPP_MOVEFINDER, m_moveFinder, moveFinder);

    if(old != NULL) {
      delete old;
    }
  }
  m_gate.signal();
  LEAVEFUNC();
}

void ChessPlayer::connect(const SocketChannel &channel) {
  ENTERFUNC();
  putRequest(channel);
  LEAVEFUNC();
}

// public
void ChessPlayer::disconnect() {
  ENTERFUNC();
  putRequest(REQUEST_DISCONNECT);
  LEAVEFUNC();
}

// public
String ChessPlayer::toString(bool detailed) const {
  String result;
  m_gate.wait();
  result = format(_T("ChessPlayer %s\n"
                     "  State           : %s\n"
                     "  Remote          : %s\n")
                 ,getPlayerNameEnglish(m_player)
                 ,STATESTR()
                 ,boolToStr(isRemote())
                 );
  String mfStr;
  if(m_moveFinder) {
    mfStr           = format(_T("%s\n"), m_moveFinder->getName().cstr());
    String stateStr = format(_T("State:\n%s")
                            ,indentString(m_moveFinder->getStateString(detailed)
                                         ,2
                                         ).cstr());
    mfStr  += indentString(stateStr, 2);
  } else {
    mfStr = _T("NULL");
  }
  result += indentString(format(_T("Movefinder      : %s"), mfStr.cstr()),2);

  m_gate.signal();
  return result;
}

// public
const TCHAR *ChessPlayer::getStateName(ChessPlayerState state) { // static 
#define caseStr(s) case CPS_##s: return _T(#s);
  switch(state) {
  caseStr(IDLE         )
  caseStr(PREPARESEARCH)
  caseStr(BUSY         )
  caseStr(STOPPENDING  )
  caseStr(MOVEREADY    )
  caseStr(KILLED       )
  default:return _T("??");
  }
#undef caseStr
}

// private
void ChessPlayer::debugMsg(const TCHAR *format, ...) const {
  va_list argptr;
  va_start(argptr, format);
  const String msg = vformat(format,argptr);
  va_end(argptr);
  verbose(_T("%s:%*.*s%s (state=%s)\n")
         ,getPlayerShortNameEnglish(m_player)
         ,m_callLevel,m_callLevel, EMPTYSTRING
         ,msg.cstr()
         ,STATESTR()
         );
}
