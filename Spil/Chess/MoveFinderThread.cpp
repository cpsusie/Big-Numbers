#include "stdafx.h"
#include "TraceDlgThread.h"
#include "MoveFinderThread.h"
#include "MoveFinderNormalPlay.h"
#include "MoveFinderRandom.h"
#include "MoveFinderEndGame.h"
#include "MoveFinderExternEngine.h"
#include "MoveFinderRemotePlayer.h"

OpeningLibrary MoveFinderThread::openingLibrary;

static TCHAR *getStateName(MoveFinderState state) {
#define caseStr(s) case MOVEFINDER_##s: return _T(#s);
  switch(state) {
  caseStr(BUSY     )
  caseStr(IDLE     )
  caseStr(MOVEREADY)
  caseStr(ERROR    )
  caseStr(KILLED   )
  default:return _T("??");
  }
#undef caseStr
}

#define STATESTR() getStateName(getState())

#ifdef _DEBUG

static TCHAR *getRequestName(MoveFinderRequest request) {
#define caseStr(s) case REQUEST_##s: return _T(#s);
  switch(request) {
  caseStr(FINDMOVE)
  caseStr(NULLMOVE)
  caseStr(RESET   )
  caseStr(KILL    )
  default:return _T("??");
  }
#undef caseStr
}

#define DEBUGMSG(msg) { verbose(_T("MoveFinderThread(%s):"),getPlayerNameEnglish(m_player)); verbose msg; }
#else
#define DEBUGMSG(msg)
#endif

MoveFinderThread::MoveFinderThread(Player player) : m_player(player), m_notBusy(0) {
  setDeamon(true);
  m_state                = MOVEFINDER_IDLE;
  m_hint                 = false;
  m_moveFinder           = NULL;
}

MoveFinderThread::~MoveFinderThread() {
  resetMoveFinder();
  putRequest(REQUEST_KILL);
  while(stillActive()) {
    Sleep(20);
  }
}

void MoveFinderThread::resetMoveFinder() {
  stopThinking();
  putRequest(REQUEST_RESET);
}

void MoveFinderThread::startThinking(const Game &game, const TimeLimit &timeLimit, bool hint) {
  const bool talking = getOptions().getTraceWindowVisible();
  if(talking) {
    clearVerbose();
  }

  DEBUGMSG((_T("enter startThinking. State:%s\n"), STATESTR()))

  const GameResult gr = game.findGameResult();
  if(isBusy()) {
    verbose(_T("startThinking called while already busy\n"));
  } else if(gr != NORESULT) {
    putRequest(REQUEST_NULLMOVE);
  } else {
    m_game                 = game;
    m_timeLimit            = timeLimit;
    m_hint                 = hint;
    m_bestMove.setNoMove();
    putRequest(REQUEST_FINDMOVE);

    DEBUGMSG((_T("leave startThinking(). State:%s\n"), STATESTR()))
  }
}

void MoveFinderThread::stopThinking(bool stopImmediately) {
  if(isBusy()) {
    DEBUGMSG((_T("enter stopThinking(Immediately=%s). State:%s\n"), boolToStr(stopImmediately), STATESTR()))
    if(m_moveFinder) {
      m_moveFinder->stopThinking(stopImmediately);
    }
    while(isBusy()) {
      DEBUGMSG((_T("wait until not busy. State=%s\n"), STATESTR()))
      m_notBusy.wait();
    }
    DEBUGMSG((_T("leave stopThinking. State:%s\n"), STATESTR()))
  }
}

void MoveFinderThread::moveNow() {
  stopThinking(false);
}

const OpeningLibrary &MoveFinderThread::getOpeningLibrary() { // static 
  if(!openingLibrary.isLoaded()) {
    openingLibrary.load(IDR_OPENINGLIBRARY);
  }
  return openingLibrary;
}

unsigned int MoveFinderThread::run() {
  setSelectedLanguageForThread();

  while(getState() != MOVEFINDER_KILLED) {
    DEBUGMSG((_T("waiting for request. State:%s\n"), STATESTR()))

    const MoveFinderRequest request = m_inputQueue.get();

    DEBUGMSG((_T("got request %s\n"), getRequestName(request)))

    setState(MOVEFINDER_BUSY);

    switch(request) {
    case REQUEST_FINDMOVE:
      try {
        m_bestMove = findMove();
        setState((m_moveFinder && (m_moveFinder->getStopCode() & STOP_IMMEDIATELY)) ? MOVEFINDER_IDLE : MOVEFINDER_MOVEREADY);
      } catch(TcpException e) {
        m_errorMessage = e.what();
        disconnect();
        setState(MOVEFINDER_ERROR);
      } catch(Exception e) {
        m_errorMessage = e.what();
        setState(MOVEFINDER_ERROR);
      } catch(CSimpleException *e) {
        TCHAR msg[1024];
        e->GetErrorMessage(msg, sizeof(msg));
        e->Delete();
        m_errorMessage = msg;
        setState(MOVEFINDER_ERROR);
      } catch(...) {
        m_errorMessage = format(_T("Unknown Exception. State=%s"), STATESTR());
        setState(MOVEFINDER_ERROR);
      }
      break;

    case REQUEST_NULLMOVE:
      m_bestMove.setNoMove();
      setState(MOVEFINDER_MOVEREADY);
      break;

    case REQUEST_RESET:
      setMoveFinder(NULL);
      setState(MOVEFINDER_IDLE);
      break;
       
    case REQUEST_KILL:
      DEBUGMSG((_T("killed\n"), getPlayerNameEnglish(m_player)))
      setState(MOVEFINDER_KILLED);
      break;
    }
  }
  return 0;
}

ExecutableMove MoveFinderThread::findMove() {
  const bool talking = getOptions().getTraceWindowVisible();

  if(getOptions().isOpeningLibraryEnabled() && !isRemote()) {
    const ExecutableMove libMove = getOpeningLibrary().findLibraryMove(m_game, talking);
    if(libMove.isMove()) {
      setMoveFinder(NULL);
      return libMove;
    }
  }

  if(newMoveFinderNeeded()) {
    allocateMoveFinder();
  }
  if(m_moveFinder != NULL) {
    return m_moveFinder->findBestMove(m_game, m_timeLimit, talking, m_hint);
  } else {
    throwException(_T("No moveFinder allocated"));
  }
  return ExecutableMove();
}

bool MoveFinderThread::newMoveFinderNeeded() const {
  if(m_moveFinder == NULL) {
    return true;
  }
  if(isRemote()) {
    return !m_moveFinder->isRemote();
  }

  const PositionType gamePositionType = m_game.getPositionType();
  switch(gamePositionType) {
  case NORMAL_POSITION      :
    return !isRightNormalPlayMoveFinder();

  case DRAW_POSITION     :
    return m_moveFinder->getPositionType() != NORMAL_POSITION;

  case TABLEBASE_POSITION    :
    { EndGameTablebase *tablebase = findMatchingTablebase();
      if(tablebase == NULL) {
        return !isRightNormalPlayMoveFinder();
      } else {
        return !isRightTablebaseMoveFinder(tablebase);
      }
      return false;
    }

  default               :
    throwException(_T("newMoveFinderNeeded::unknown positiontype:%d"), gamePositionType);
    return false;
  }
}

bool MoveFinderThread::isRightNormalPlayMoveFinder() const {
  if(m_moveFinder->getPositionType() != NORMAL_POSITION) {
    return false;
  }
  if(m_timeLimit.m_timeout == 0) {
    return m_moveFinder->getEngineType() == RANDOM_ENGINE;
  }

  return m_moveFinder->getEngineType() == EXTERN_ENGINE;
}

bool MoveFinderThread::isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const {
  if(m_moveFinder->getPositionType() != TABLEBASE_POSITION) {
    return false;
  }
  bool swapPlayers;
  return ((MoveFinderEndGame*)m_moveFinder)->getPositionSignature().match(tablebase->getKeyDefinition().getPositionSignature(), swapPlayers);
}

EndGameTablebase *MoveFinderThread::findMatchingTablebase() const {
  if(!getOptions().isEndGameTablebaseEnabled()) {
    return NULL;
  }
  bool swapPlayers;
  EndGameTablebase *db = EndGameTablebase::getInstanceBySignature(m_game.getPositionSignature(), swapPlayers);
  return (db && (db->exist(DECOMPRESSEDTABLEBASE) || db->exist(COMPRESSEDTABLEBASE))) ? db : NULL;
}

void MoveFinderThread::allocateMoveFinder() {
  if(isRemote()) {
    setMoveFinder(new MoveFinderRemotePlayer(getPlayer(), m_channel));
    return;
  }
  switch(m_game.getPositionType()) {
  case NORMAL_POSITION  :
  case DRAW_POSITION    :
    setMoveFinder(newMoveFinderNormalPlay());
    break;

  case TABLEBASE_POSITION:
    { EndGameTablebase *tablebase = findMatchingTablebase();
      if(tablebase == NULL) {
        setMoveFinder(newMoveFinderNormalPlay());
      } else {
        setMoveFinder(new MoveFinderEndGame(getPlayer(), tablebase));
      }
    }
    break;
  }
}

AbstractMoveFinder *MoveFinderThread::newMoveFinderNormalPlay() {
  if(m_timeLimit.m_timeout == 0) {
    return new MoveFinderRandomPlay(getPlayer());
  } else {
    return new MoveFinderExternEngine(getPlayer());
  }
}

ExecutableMove MoveFinderThread::getMove() const {
  if(getState() != MOVEFINDER_MOVEREADY) {
    throwException(_T("No move ready. State=%s"), STATESTR());
  }
  return m_bestMove;
}

void MoveFinderThread::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(id == TRACEWINDOW_ACTIVE) {
    m_gate.wait();
    if(m_moveFinder != NULL) {
      const bool verbose = *(const bool*)newValue;
      m_moveFinder->setVerbose(verbose);
    }
    m_gate.signal();
  }
}

PositionType MoveFinderThread::getPositionType() const {
  return m_moveFinder ? m_moveFinder->getPositionType() : m_game.getPositionType();
}

bool MoveFinderThread::notifyGameChanged(const Game &game) {
  try {
    if(m_moveFinder) {
      m_moveFinder->notifyGameChanged(game);
    }
    return true;
  } catch(TcpException e) {
    handleTcpException(e);
    return false;
  }
}

bool MoveFinderThread::notifyMove(const MoveBase &move) {
  try {
    if(m_moveFinder) {
      m_moveFinder->notifyMove(move);
    }
    return true;
  } catch(TcpException e) {
    handleTcpException(e);
    return false;
  }
}

bool MoveFinderThread::acceptUndoMove() {
  try {
    return m_moveFinder ? m_moveFinder->acceptUndoMove() : true;
  } catch(TcpException e) {
    handleTcpException(e);
    return false;
  }
}

void MoveFinderThread::handleTcpException(const TcpException &e) {
  m_errorMessage = e.what();
  disconnect();
  setState(MOVEFINDER_ERROR);
}

String MoveFinderThread::getName() const {
  if(m_moveFinder) {
    return m_moveFinder->getName();
  } else {
    return _T("none");
  }
}

void MoveFinderThread::setState(MoveFinderState newState) {
  if(newState != m_state) {
    m_gate.wait();
    const MoveFinderState oldState = m_state;
    m_state                        = newState;

    DEBUGMSG((_T("state %s -> %s\n"), getStateName(oldState), getStateName(newState)));

    notifyPropertyChanged(MOVEFINDER_STATE, &oldState, &newState);
    m_gate.signal();

    if(newState != MOVEFINDER_BUSY) {
      m_notBusy.signal();
    }
  }
}

void MoveFinderThread::setMoveFinder(AbstractMoveFinder *moveFinder) {
  if(moveFinder != m_moveFinder) {
    m_gate.wait();

    const AbstractMoveFinder *oldValue = m_moveFinder;
    m_moveFinder                       = moveFinder;
    notifyPropertyChanged(MOVEFINDER_ENGINE, oldValue, m_moveFinder);
  
    if(oldValue != NULL) {
      delete oldValue;
    }
    m_gate.signal();
  }
}

void MoveFinderThread::setRemote(const SocketChannel &channel) {
  const bool oldRemote = isRemote();
  m_channel = channel;
  const bool newRemote = isRemote();
  if(newRemote != oldRemote) {
    m_gate.wait();

    notifyPropertyChanged(MOVEFINDER_REMOTE, &oldRemote, &newRemote);

    m_gate.signal();

    if(newMoveFinderNeeded()) {
      allocateMoveFinder();
    }
  }
}

void MoveFinderThread::disconnect() {
  setRemote(SocketChannel());
}

void MoveFinderThread::printState(Player computerPlayer, bool detailed) {
  String result;
  m_gate.wait();
  result = format(_T("MoveFinderThread\n"
                     "   state           : %s\n"
                     "   Timeout         : %s\n"
                     "   Hint            : %s\n"
                     "   Remote          : %s\n")
                 ,STATESTR()
                 ,m_timeLimit.toString().cstr()
                 ,boolToStr(m_hint)
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
