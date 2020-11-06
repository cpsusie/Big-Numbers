#pragma once

#include <Runnable.h>
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

class ChessPlayer : public Runnable, public PropertyContainer, public PropertyChangeListener, OptionsAccessor {
private:

  static OpeningLibrary s_openingLibrary;

  const Player            m_player;
  ChessPlayerState        m_state;
  ChessPlayerRequestQueue m_inputQueue;
  AbstractMoveFinder     *m_moveFinder;
  SocketChannel           m_channel;
  SearchMoveResult        m_searchResult;
  String                  m_messageText;
  mutable Semaphore       m_lock, m_terminated;
  mutable BYTE            m_callLevel;

  void putRequest(ChessPlayerRequest request) {
    m_inputQueue.put(request);
  }
  bool isNewMoveFinderNeeded(      const RequestParamFindMove &param) const;
  bool isRightNormalPlayMoveFinder(const RequestParamFindMove &param) const;
  void allocateMoveFinder(         const RequestParamFindMove &param);
  void allocateRemoteMoveFinder();
  AbstractMoveFinder *newMoveFinderNormalPlay(const RequestParamFindMove &param);
  bool isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const;
  EndGameTablebase *findMatchingTablebase(const Game &g) const;
  void handleRequestFindMove(   const RequestParamFindMove    &param);
  void dohandleRequestFindMove( const RequestParamFindMove    &param);
  void handleRequestNullMove();
  void handleRequestStopSearch();
  void handleRequestMoveNow();
  void handleRequestGameChanged(const RequestParamGameChanged &param);
  void handleRequestMoveDone(   const RequestParamMoveDone    &param);
  void handleRequestShowMessage(const RequestParamShowMessage &param);
  void handleRequestFetchMove(  const RequestParamFetchMove   &param);
  void handleRequestReset();
  void handleRequestConnect(    const RequestParamConnect     &param);
  void handleRequestDisconnect();
  void handleRequestKill();
  inline ChessPlayerState getState() const {
    return m_state;
  }
  void setState(     ChessPlayerState                   newState  );
  void setMoveFinder(AbstractMoveFinder                *moveFinder);
  void setRemote(const SocketChannel &channel);
  void handleTcpException(const TcpException &e);
  void handleException(   const Exception    &e);
  void handleUnknownException(const TCHAR *method);
  void checkState(const TCHAR *method, int line, ChessPlayerState s1,...) const;
  void debugMsg(_In_z_ _Printf_format_string_ TCHAR const * const format,...) const;
public:
  ChessPlayer(Player player);
  ~ChessPlayer();
  void start();
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
