#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>
#include "OpeningLibrary.h"
#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"
#include "SocketChannel.h"

typedef enum {
  CPS_IDLE            // Start state of ChessPlayer. Goes here whenever ready for work
 ,CPS_PREPARESEARCH   // Thread is preparing a search for best move
 ,CPS_BUSY            // ChessPlayer is busy finding a move
 ,CPS_STOPPENDING     // MoveFinder is busy, and has received REQUEST_STOPSEARCH (NOT MOVENOW)
 ,CPS_MOVEREADY       // Thread has got a new move ready for the dialog
 ,CPS_KILLED          // ChessPlayer run as long state is not CPS_KILLED
} ChessPlayerState;

typedef enum {
  CPP_STATE           // ChessPlayerState.   Id of notifications when ChessPlayer.m_state changes
 ,CPP_REMOTE          // bool.               Id of notifications when ChessPlayer.m_channel goes from disconnected -> connected or vice versa
 ,CPP_MOVEFINDER      // AbstractMoveFinder* Id of notifications when ChessPlayer.m_moveFinder is changed
 ,CPP_MESSAGETEXT     // String*             Id of notification  when ChessPlayer.m_msg is changed
} ChessPlayerProperty;

class ChessPlayer : public Thread, public PropertyContainer, public PropertyChangeListener, OptionsAccessor {
private:

  static OpeningLibrary s_openingLibrary;

  const Player            m_player;
  ChessPlayerState        m_state;
  ChessPlayerRequestQueue m_inputQueue;
  AbstractMoveFinder     *m_moveFinder;
  SocketChannel           m_channel;
  SearchMoveResult        m_searchResult;
  String                  m_messageText;
  mutable Semaphore       m_gate;
  mutable BYTE            m_callLevel;

  void putRequest(ChessPlayerRequest request) {
    m_inputQueue.put(request);
  }
  bool isNewMoveFinderNeeded(      const FindMoveRequestParam &param) const;
  bool isRightNormalPlayMoveFinder(const FindMoveRequestParam &param) const;
  void allocateMoveFinder(         const FindMoveRequestParam &param);
  void allocateRemoteMoveFinder();
  AbstractMoveFinder *newMoveFinderNormalPlay(const FindMoveRequestParam &param);
  bool isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const;
  EndGameTablebase *findMatchingTablebase(const Game &g) const;
  void handleFindMoveRequest(   const FindMoveRequestParam    &param);
  void handleNullMoveRequest();
  void handleStopSearchRequest();
  void handleMoveNowRequest();
  void handleGameChangedRequest(const GameChangedRequestParam &param);
  void handleMoveDoneRequest(   const MoveDoneRequestParam    &param);
  void handleShowMessageRequest(const ShowMessageRequestParam &param);
  void handleFetchMoveRequest(  const FetchMoveRequestParam   &param);
  void handleResetRequest();
  void handleConnectRequest(    const ConnectRequestParam     &param);
  void handleDisconnectRequest();
  void handleKillRequest();
  inline ChessPlayerState getState() const {
    return m_state;
  }
  void setState(     ChessPlayerState                   newState  );
  void setMoveFinder(AbstractMoveFinder                *moveFinder);
  void setRemote(const SocketChannel &channel);
  void handleTcpException(const TcpException &e);
  void checkState(const TCHAR *method, int line, ChessPlayerState s1,...) const;
  void debugMsg(const TCHAR *format,...) const;
public:
  ChessPlayer(Player player);
  ~ChessPlayer();
  UINT run();

  inline Player getPlayer() const {
    return m_player;
  }
  inline bool isBusy() const {
    return (m_state == CPS_BUSY) || (m_state == CPS_STOPPENDING);
  }

  void startSearch(const Game &game, const TimeLimit &timeLimit, bool hint);
  void stopSearch();
  void resetMoveFinder();

  // Only valid in state CPS_BUSY
  void moveNow();

  // Only valid in state CPS_MOVEREADY
  SearchMoveResult getSearchResult() const;

  void notifyGameChanged(const Game &game);     

  // Only valid in state CPS_IDLE
  void notifyMove(const PrintableMove &m);

  // Only valid in state CPS_IDLE
  bool acceptUndoMove();

  inline bool isRemote() const {
    return m_channel.isOpen();
  }

  // Only valid in state CPS_IDLE
  void connect(const SocketChannel &channel);

  // Only valid in state CPS_IDLE
  void disconnect();                            

  static const OpeningLibrary &getOpeningLibrary();

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  String toString(bool detailed) const;
  String getName() const;
  static const TCHAR *getStateName(ChessPlayerState state);
};
