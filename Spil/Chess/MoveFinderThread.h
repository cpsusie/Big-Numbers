#pragma once

#include <Thread.h>
#include <SynchronizedQueue.h>
#include "OpeningLibrary.h"
#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"
#include "SocketChannel.h"

typedef enum {
  MOVEFINDER_IDLE
 ,MOVEFINDER_BUSY
 ,MOVEFINDER_MOVEREADY
 ,MOVEFINDER_ERROR
 ,MOVEFINDER_KILLED
} MoveFinderState;

typedef enum {
  MOVEFINDER_STATE
 ,MOVEFINDER_REMOTE
 ,MOVEFINDER_CONNECTED
 ,MOVEFINDER_ENGINE
} MoveFinderProperty;

typedef enum {
  REQUEST_FINDMOVE
 ,REQUEST_NULLMOVE
 ,REQUEST_RESET
 ,REQUEST_KILL
} MoveFinderRequest;

class MoveFinderThread : public Thread, public PropertyContainer, public PropertyChangeListener, OptionsAccessor {
private:

  static OpeningLibrary s_openingLibrary;

  const Player                         m_player;
  MoveFinderState                      m_state;
  SynchronizedQueue<MoveFinderRequest> m_inputQueue;
  Game                                 m_game;
  TimeLimit                            m_timeLimit;
  bool                                 m_hint;
  ExecutableMove                       m_bestMove;
  String                               m_errorMessage;
  Semaphore                            m_notBusy, m_gate;
  AbstractMoveFinder                  *m_moveFinder;
  SocketChannel                        m_channel;

  void putRequest(MoveFinderRequest request) {
    m_inputQueue.put(request);
  }
  bool newMoveFinderNeeded() const;
  bool isRightNormalPlayMoveFinder() const;
  bool isRightTablebaseMoveFinder(EndGameTablebase *tablebase) const;
  EndGameTablebase *findMatchingTablebase() const;
  AbstractMoveFinder *newMoveFinderNormalPlay();
  void allocateMoveFinder();
  ExecutableMove findMove();
  MoveFinderState getState() const {
    return m_state;
  }
  void setState(     MoveFinderState     newState  );
  void setMoveFinder(AbstractMoveFinder *moveFinder);
  void handleTcpException(const TcpException &e);
public:
  MoveFinderThread(Player player);
  ~MoveFinderThread();
  UINT run();

  Player getPlayer() const {
    return m_player;
  }
  bool isBusy() const {
    return getState() == MOVEFINDER_BUSY;
  }

  bool isHint() const {
    return m_hint;
  }
  ExecutableMove getMove() const;
  void startThinking(const Game &game, const TimeLimit &timeLimit, bool hint);
  void stopThinking(bool stopImmediately = true);
  void resetMoveFinder();
  void moveNow();
  bool notifyGameChanged(const Game &game);
  bool notifyMove(const MoveBase &m);
  bool acceptUndoMove();

  bool isRemote() const {
    return m_channel.isOpen();
  }
  void setRemote(const SocketChannel &channel);
  void disconnect();
  static const OpeningLibrary &getOpeningLibrary();

  PositionType getPositionType() const;
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  void printState(Player computerPlayer, bool detailed);
  String getName() const;
  const String &getErrorMessage() const {
    return m_errorMessage;
  }
};
