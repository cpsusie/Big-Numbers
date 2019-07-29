#include "stdafx.h"
#include "TraceDlgThread.h"
#include "ChessPlayer.h"
#include "MoveFinderRandom.h"
#include "MoveFinderEndGame.h"
#include "MoveFinderExternEngine.h"
#include "MoveFinderRemotePlayer.h"

//#define _PRINT_DEBUGMSG
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

#define ENTERFUNC() ENTERFUNCPARAM(EMPTYSTRING)

#define LEAVEFUNC() {                            \
  m_callLevel-= 2;                               \
  debugMsg(_T("Leave %s"),__TFUNCTION__);        \
}

#else

#define ENTERFUNCPARAM(...)
#define ENTERFUNC()
#define LEAVEFUNC()

#endif // _TRACE_ENTERLEAVE

#define ENTER_LOCK()        \
  ENTERFUNC();              \
  m_gate.wait()

#define UNLOCK_LEAVE()      \
  m_gate.signal();          \
  LEAVEFUNC()

#define UNLOCK_RETURN(expr) \
  UNLOCK_LEAVE();           \
  return expr

#ifdef CATCH_ALL
#undef CATCH_ALL
#endif

#define CATCH_ALL()                                                                             \
catch(TcpException e) {                                                                         \
  handleTcpException(e);                                                                        \
} catch(Exception e) {                                                                          \
  handleException(e);                                                                           \
} catch(CSimpleException *e) {                                                                  \
  TCHAR msg[1024];                                                                              \
  e->GetErrorMessage(msg, sizeof(msg));                                                         \
  e->Delete();                                                                                  \
  putRequest(ChessPlayerRequest(msg, true));                                                    \
} catch(...) {                                                                                  \
  handleUnknownException(__TFUNCTION__);                                                        \
}

#define CATCH_UNLOCK_RETHROW() \
catch (...) {                  \
  m_gate.signal();             \
  LEAVEFUNC();                 \
  throw;                       \
}

#define CATCH_LEAVE_RETHROW()  \
catch (...) {                  \
  LEAVEFUNC();                 \
  throw;                       \
}

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
void ChessPlayer::startSearch(const Game &game, const TimeLimit &timeLimit, bool hint) {
  ENTERFUNC();
  const bool talking = getOptions().getTraceWindowVisible();
  if(talking) {
    clearVerbose();
  }
  const GameResult gr = game.findGameResult();
  if(isBusy()) {
    putRequest(ChessPlayerRequest(format(_T("%s called while already busy"), __TFUNCTION__),true)); // REQUEST_SHOWMESSAGE
  } else if(gr != NORESULT) {
    putRequest(REQUEST_NULLMOVE);
  } else {
    m_gate.wait();
    m_searchResult.m_move.setNoMove();
    putRequest(ChessPlayerRequest(game, timeLimit, hint, talking)); // REQUEST_FINDMOVE
    m_gate.signal();
  }
  LEAVEFUNC();
}

// public
void ChessPlayer::stopSearch() {
  ENTER_LOCK();
  switch(getState()) {
  case CPS_IDLE         :
    break;
  case CPS_PREPARESEARCH:
    setState(CPS_IDLE);
    break;
  case CPS_BUSY         :
    setState(CPS_STOPPENDING);
    putRequest(REQUEST_STOPSEARCH);
    break;
  case CPS_MOVEREADY    :
  case CPS_STOPPENDING  :
  case CPS_KILLED       :
    break;
  }
  UNLOCK_LEAVE();
}

// public
void ChessPlayer::moveNow() {
  ENTER_LOCK();
  if(getState() == CPS_BUSY) {
    putRequest(REQUEST_MOVENOW);
  }
  UNLOCK_LEAVE();
}

// public. Get the searchResult. Only valid if state if CPS_MOVEREADY
SearchMoveResult ChessPlayer::getSearchResult() const {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_MOVEREADY);
    const SearchMoveResult result = m_searchResult;
    UNLOCK_RETURN(result);
  } CATCH_UNLOCK_RETHROW();
}

// public
void ChessPlayer::notifyGameChanged(const Game &game) {
  ENTERFUNC();
  try {
    CHECKSTATE(CPS_IDLE, CPS_MOVEREADY, CPS_STOPPENDING);
    putRequest(ChessPlayerRequest(game));   // REQUEST_GAMECHANGED
    LEAVEFUNC();
  } CATCH_LEAVE_RETHROW();
}

// public
void ChessPlayer::notifyMove(const PrintableMove &m) {
  ENTERFUNC();
  try {
    CHECKSTATE(CPS_IDLE, CPS_MOVEREADY);
    putRequest(ChessPlayerRequest(m));      // REQUEST_MOVEDONE
    LEAVEFUNC();
  } CATCH_LEAVE_RETHROW();
}

// public
void ChessPlayer::resetMoveFinder() {
  ENTERFUNC();
  putRequest(REQUEST_RESET);
  LEAVEFUNC();
}

// public
void ChessPlayer::connect(const SocketChannel &channel) {
  ENTERFUNC();
  putRequest(channel);    // REQUEST_CONNECT
  LEAVEFUNC();
}

// public
void ChessPlayer::disconnect() {
  ENTERFUNC();
  putRequest(REQUEST_DISCONNECT);
  LEAVEFUNC();
}

// public
bool ChessPlayer::acceptUndoMove() {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE,CPS_PREPARESEARCH,CPS_BUSY,CPS_MOVEREADY);
    const bool result = m_moveFinder ? m_moveFinder->acceptUndoMove() : true;
    UNLOCK_RETURN(result);
  } catch(TcpException e) {
    handleTcpException(e);
    UNLOCK_RETURN(false);
  } CATCH_UNLOCK_RETHROW();
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
    case REQUEST_FINDMOVE   :
      handleRequestFindMove(request.getParamFindMove());
      break;

    case REQUEST_NULLMOVE   :
      handleRequestNullMove();
      break;

    case REQUEST_STOPSEARCH :
      handleRequestStopSearch();
      break;

    case REQUEST_MOVENOW    :
      handleRequestMoveNow();
      break;

    case REQUEST_FETCHMOVE  :
      handleRequestFetchMove(request.getParamFetchMove());
      break;

    case REQUEST_GAMECHANGED:
      handleRequestGameChanged(request.getParamGameChanged());
      break;

    case REQUEST_MOVEDONE   :
      handleRequestMoveDone(request.getParamMoveDone());
      break;

    case REQUEST_SHOWMESSAGE:
      handleRequestShowMessage(request.getParamShowMessage());
      break;

    case REQUEST_RESET      :
      handleRequestReset();
      break;

    case REQUEST_CONNECT    :
      handleRequestConnect(request.getParamConnect());
      break;

    case REQUEST_DISCONNECT :
      handleRequestDisconnect();
      break;

    case REQUEST_KILL       :
      handleRequestKill();
      break;

    default                 :
      debugMsg(_T("Invalid request:%s"), request.toString().cstr());
      break;
    }
  }
  LEAVEFUNC();
  return 0;
}

void ChessPlayer::handleRequestFindMove(const RequestParamFindMove &param) {
  try {
    dohandleRequestFindMove(param);
  } CATCH_ALL();
}

// private
void ChessPlayer::dohandleRequestFindMove(const RequestParamFindMove &param) {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE,CPS_MOVEREADY);
    setState(CPS_PREPARESEARCH);
    m_gate.signal();
  } CATCH_UNLOCK_RETHROW();

  if(getOptions().isOpeningLibraryEnabled() && !isRemote()) {
    const PrintableMove libMove = getOpeningLibrary().findLibraryMove(param.getGame(), param.isVerbose());
    if(libMove.isMove()) {
      setMoveFinder(NULL);
      m_gate.wait();
      if(getState() == CPS_PREPARESEARCH) {
        setState(CPS_BUSY);
        putRequest(ChessPlayerRequest(libMove, param.isHint()));
      } else {
        // do nothing!! User has pressed undo, so state is idle
      }
      UNLOCK_LEAVE();
      return;
    }
  }

  if(isNewMoveFinderNeeded(param)) {
    allocateMoveFinder(param);
  }

  m_gate.wait();
  try {
    if(m_moveFinder != NULL) {
      if(getState() == CPS_PREPARESEARCH) {
        setState(CPS_BUSY);
        m_moveFinder->findBestMove(param);
      } else {
        // do nothing!! User has pressed undo, so state is idle
      }
    } else {
      setState(CPS_IDLE);
      throwException(_T("No moveFinder allocated"));
    }
    UNLOCK_LEAVE();
  } CATCH_UNLOCK_RETHROW();
}

// private
void ChessPlayer::handleRequestNullMove() {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE, CPS_MOVEREADY);
    if(getState() == CPS_MOVEREADY) {
      setState(CPS_IDLE);
    }
    m_searchResult.clear();
    setState(CPS_MOVEREADY);
    UNLOCK_LEAVE();
  } CATCH_UNLOCK_RETHROW();
}

// private
void ChessPlayer::handleRequestStopSearch() {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE,CPS_STOPPENDING);
    if(getState() == CPS_STOPPENDING) {
      if(m_moveFinder) {
        m_moveFinder->stopSearch();
      }
      setState(CPS_IDLE);
    }
    UNLOCK_LEAVE();
  } CATCH_UNLOCK_RETHROW();
}

// private
void ChessPlayer::handleRequestMoveNow() {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE, CPS_BUSY);
    if((getState() == CPS_BUSY) && m_moveFinder) {
      m_moveFinder->moveNow();
    }
    UNLOCK_LEAVE();
  } CATCH_UNLOCK_RETHROW();
}

// private
void ChessPlayer::handleRequestFetchMove(const RequestParamFetchMove &param) {
  ENTER_LOCK();
  try {
    switch(getState()) {
    case CPS_IDLE       :
      break;
    case CPS_STOPPENDING:
      setState(CPS_IDLE);
      break;
    case CPS_BUSY       :
      m_searchResult = param.getSearchResult();
      setState(m_searchResult.isMove() ? CPS_MOVEREADY : CPS_IDLE);
      break;
    default              :
      CHECKSTATE(CPS_IDLE, CPS_BUSY, CPS_STOPPENDING);
      break;
    }
    UNLOCK_LEAVE();
  } CATCH_UNLOCK_RETHROW();
}

// private
void ChessPlayer::handleRequestGameChanged(const RequestParamGameChanged &param) {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE, CPS_MOVEREADY);

    if(m_moveFinder) {
      m_moveFinder->notifyGameChanged(param.getGame());
    }
    setState(CPS_IDLE);
  } CATCH_ALL();
  UNLOCK_LEAVE();
}

// private
void ChessPlayer::handleRequestMoveDone(const RequestParamMoveDone &param) {
  ENTER_LOCK();
  try {
    CHECKSTATE(CPS_IDLE,CPS_MOVEREADY);
    if(m_moveFinder) {
      m_moveFinder->notifyMove(param.getMove());
    }
    setState(CPS_IDLE);
  } CATCH_ALL();
  UNLOCK_LEAVE();
}

// private
void ChessPlayer::handleRequestShowMessage(const RequestParamShowMessage &param) {
  setProperty(CPP_MESSAGETEXT, m_messageText, param.getMessage());
  if (param.isError()) {
    putRequest(REQUEST_RESET);
  }
}

// private
void ChessPlayer::handleRequestReset() {
  ENTERFUNC();
  setMoveFinder(NULL);
  setProperty(CPP_MESSAGETEXT, m_messageText, EMPTYSTRING);
  setState(CPS_IDLE);
  LEAVEFUNC();
}

// private
void ChessPlayer::handleRequestConnect(const RequestParamConnect &param) {
  ENTERFUNC();
  setRemote(param.getChannel());
  LEAVEFUNC();
}

// private
void ChessPlayer::handleRequestDisconnect() {
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
void ChessPlayer::handleRequestKill() {
  ENTER_LOCK();
  CHECKSTATE(CPS_IDLE);
  setState(CPS_KILLED);
  UNLOCK_LEAVE();
}

// private
void ChessPlayer::handleTcpException(const TcpException &e) {
  ENTERFUNC();
  disconnect();
  putRequest(ChessPlayerRequest(e.what(), true));
  LEAVEFUNC();
}

void ChessPlayer::handleException(const Exception &e) {
  ENTERFUNC();
  putRequest(ChessPlayerRequest(e.what(), true));
  LEAVEFUNC();
}

void ChessPlayer::handleUnknownException(const TCHAR *method) {
  ENTERFUNC();
  putRequest(ChessPlayerRequest(format(_T("Unknown Exception in %s (State=%s)")
                                      ,method, STATESTR()), true));
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
bool ChessPlayer::isNewMoveFinderNeeded(const RequestParamFindMove &param) const {
  if(m_moveFinder == NULL) {
    return true;
  }
  if(isRemote()) {
    return m_moveFinder->getType() != REMOTE_PLAYER;
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
bool ChessPlayer::isRightNormalPlayMoveFinder(const RequestParamFindMove &param) const {
  if(m_moveFinder->getPositionType() != NORMAL_POSITION) {
    return false;
  }

#ifndef TABLEBASE_BUILDER
  if(param.getTimeLimit().m_timeout == 0) {
    return m_moveFinder->getType() == RANDOM_PLAYER;
  }
  return m_moveFinder->getType() == EXTERN_ENGINE;
#else
  return m_moveFinder->getType() == RANDOM_PLAYER;
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
  return (db && (db->tbFileExist(DECOMPRESSEDTABLEBASE) || db->tbFileExist(COMPRESSEDTABLEBASE))) ? db : NULL;
}

void ChessPlayer::allocateRemoteMoveFinder() {
  assert(isRemote());
  AbstractMoveFinder *amf = new MoveFinderRemotePlayer(getPlayer(), m_inputQueue, m_channel); TRACE_NEW(amf);
  setMoveFinder(amf);
}

// private
void ChessPlayer::allocateMoveFinder(const RequestParamFindMove &param) {
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
        AbstractMoveFinder *amf = new MoveFinderEndGame(getPlayer(), m_inputQueue, tablebase); TRACE_NEW(amf);
        setMoveFinder(amf);
      }
    }
    break;
  }
}

// private
AbstractMoveFinder *ChessPlayer::newMoveFinderNormalPlay(const RequestParamFindMove &param) {
  AbstractMoveFinder *amf;
#ifndef TABLEBASE_BUILDER
  if(param.getTimeLimit().m_timeout == 0) {
    amf = new MoveFinderRandomPlay(getPlayer(), m_inputQueue);
  } else {
    amf = new MoveFinderExternEngine(getPlayer(), m_inputQueue);
  }
#else
  amf = new MoveFinderRandomPlay(getPlayer(), m_inputQueue);
#endif
  TRACE_NEW(amf);
  return amf;
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
  ENTER_LOCK();
  const String result = m_moveFinder ? m_moveFinder->getName() : _T("None");
  UNLOCK_RETURN(result);
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
  ENTER_LOCK();
  if(moveFinder != m_moveFinder) {

    DEBUGMSG(_T("Change movefinder(%s->%s)")
            ,m_moveFinder?m_moveFinder->getName().cstr():_T("NULL")
            ,moveFinder  ?  moveFinder->getName().cstr():_T("NULL"));

    const AbstractMoveFinder *old = m_moveFinder;
    setProperty(CPP_MOVEFINDER, m_moveFinder, moveFinder);

    SAFEDELETE(old);
  }
  UNLOCK_LEAVE();
}

// public
String ChessPlayer::toString(bool detailed) const {
  ENTER_LOCK();
  String result;
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

  UNLOCK_RETURN(result);
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
void ChessPlayer::debugMsg(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) const {
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
