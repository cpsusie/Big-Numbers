#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>
#include "OpeningLibrary.h"
#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"
#include "SocketChannel.h"

typedef enum {
  MFTS_IDLE            // Start state of MoveFinderThread. Goes here whenever ready for work
 ,MFTS_PREPARESEARCH   // Thread is preparing a search for best move
 ,MFTS_BUSY            // MoveFinderThread is busy finding a move
 ,MFTS_STOPPENDING     // MoveFinder is busy, and has received REQUEST_STOPSEARCH (NOT MOVENOW)
 ,MFTS_MOVEREADY       // Thread has got a new move ready for the dialog
 ,MFTS_ERROR           // Some error has happened to the thread
 ,MFTS_KILLED          // MoveFinderThread run as long state is not MFTS_KILLED
} MoveFinderThreadState;

typedef enum {
  MFTP_STATE           // MoveFinderThreadState. Id of notifications when MoveFinderThread.m_state changes
 ,MFTP_REMOTE          // bool.                  Id of notifications when MoveFinderThread.m_channel goes from disconnected -> connected or vice versa
 ,MFTP_MOVEFINDER      // AbstractMoveFinder*    Id of notifications when MoveFinderThread.m_moveFinder is changed
} MoveFinderThreadProperty;

class MoveFinderThread : public Thread, public PropertyContainer, public PropertyChangeListener, OptionsAccessor {
private:

  static OpeningLibrary s_openingLibrary;

  const Player          m_player;
  MoveFinderThreadState m_state;
  MFTRQueue             m_inputQueue;
  AbstractMoveFinder   *m_moveFinder;
  SocketChannel         m_channel;
  SearchMoveResult      m_searchResult;
  String                m_errorMessage;
  mutable Semaphore     m_gate;
  mutable BYTE          m_callLevel;

  void putRequest(MoveFinderThreadRequest request) {
    m_inputQueue.put(request);
  }
  bool isNewMoveFinderNeeded(      const FindMoveRequestParam &param) const;
  bool isRightNormalPlayMoveFinder(const FindMoveRequestParam &param) const;
  void allocateMoveFinder(         const FindMoveRequestParam &param);
  AbstractMoveFinder *newMoveFinderNormalPlay(const FindMoveRequestParam &param);
  bool isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const;
  EndGameTablebase *findMatchingTablebase(const Game &g) const;
  void handleFindMoveRequest(   FindMoveRequestParam    param);
  void handleNullMoveRequest();
  void handleStopSearchRequest();
  void handleMoveNowRequest();
  void handleGameChangedRequest(const Game             &game);
  void handleFetchMoveRequest(  const SearchMoveResult &searchResult);
  void handleResetRequest();
  void handleDisconnectRequest();
  void handleKillRequest();
  inline MoveFinderThreadState getState() const {
    return m_state;
  }
  void setState(     MoveFinderThreadState  newState  );
  void setMoveFinder(AbstractMoveFinder    *moveFinder);
  void handleTcpException(const TcpException &e);
  void checkState(const TCHAR *method, int line, MoveFinderThreadState s1,...) const;
  void debugMsg(const TCHAR *format,...) const;

public:
  MoveFinderThread(Player player);
  ~MoveFinderThread();
  UINT run();

  Player getPlayer() const {
    return m_player;
  }
  inline bool isBusy() const {
    return (m_state == MFTS_BUSY) || (m_state == MFTS_STOPPENDING);
  }

  void startThinking(const Game &game, const TimeLimit &timeLimit, bool hint);
  void stopSearch();
  void resetMoveFinder();

  // Only valid in state MFTS_BUSY
  void moveNow();

  // Only valid in state MFTS_MOVEREADY
  SearchMoveResult getSearchResult() const;

  void notifyGameChanged(const Game &game);     

  // Only valid in state MFTS_IDLE
  bool notifyMove(const MoveBase &m);

  // Only valid in state MFTS_IDLE
  bool acceptUndoMove();

  inline bool isRemote() const {
    return m_channel.isOpen();
  }

  // Only valid in state MFTS_IDLE
  void setRemote(const Game &game, const SocketChannel &channel);

  // Only valid in state MFTS_IDLE
  void disconnect();                            

  static const OpeningLibrary &getOpeningLibrary();

  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void printState(Player computerPlayer, bool detailed);
  String getName() const;
  const String &getErrorMessage() const {
    return m_errorMessage;
  }
  static const TCHAR *getStateName(  MoveFinderThreadState   state  );
};
