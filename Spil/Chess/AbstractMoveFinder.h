#pragma once

#include "ChessPlayerRequest.h"

typedef enum {
  INTERN_ENGINE
 ,EXTERN_ENGINE
 ,REMOTE_ENGINE
 ,RANDOM_ENGINE
} EngineType;

class AbstractMoveFinder : public AbstractMoveReceiver {
private:
  bool                     m_verbose;
  const Player             m_player;
  ChessPlayerRequestQueue &m_msgQueue;
protected:
  Game                     m_game;
  TimeLimit                m_timeLimit;
  bool                     m_hint;

  void initSearch(const FindMoveRequestParam &param);
  bool isVerbose() {
    return m_verbose;
  }
  PrintableMove checkForSingleMove();
public:
  // msgQueue is for sending messages back to owner.
  // Dont get message from it!! only put
  AbstractMoveFinder(Player player, ChessPlayerRequestQueue &msgQueue);
  virtual ~AbstractMoveFinder() {
  }
  Player getPlayer() const {
    return m_player;
  }
  void putMove(const MoveBase &m);
  void putError(const TCHAR *fmt,...);
  // Should return immediately. dont wait for engine to finish search
  virtual void           findBestMove(const FindMoveRequestParam &param) = 0;
  virtual void           stopSearch()               = 0;
  virtual void           moveNow()                  = 0;
  virtual String         getName()            const = 0;
  virtual EngineType     getEngineType()      const = 0;
  bool                   isRemote()           const {
    return getEngineType() == REMOTE_ENGINE;
  }
  virtual PositionType   getPositionType()    const = 0;
  virtual void           setVerbose(bool verbose) {
    m_verbose = verbose;
  }
  virtual String getStateString(bool detailed) = 0;
  virtual void notifyGameChanged(const Game &game) {
  };
  virtual void notifyMove(const MoveBase &move) {
  }
  virtual bool acceptUndoMove() {
    return true;
  }
};
