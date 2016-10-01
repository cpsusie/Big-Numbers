#pragma once

#define MIN_SCORE -9999
#define MAX_SCORE  9999

typedef enum {
  INTERN_ENGINE
 ,EXTERN_ENGINE
 ,REMOTE_ENGINE
 ,RANDOM_ENGINE
} EngineType;

#define STOP_WHENMOVEFOUND 0x01
#define STOP_IMMEDIATELY   0x02
#define STOPPED_BY_USER    0x04

typedef enum {
  CMD_GETGAME
 ,CMD_GETMOVE
 ,CMD_ASKACCEPTUNDO
 ,CMD_UNDOACCEPTED
 ,CMD_UNDOREFUSED
 ,CMD_RESIGN
 ,CMD_OFFERDRAW
 ,CMD_INTERRUPT
} MoveFinderCommand;

class AbstractMoveFinder {
private:
  bool              m_verbose;
  const Player      m_player;
protected:
  Game             *m_game;
  TimeLimit         m_timeLimit;
  unsigned char     m_stopCode;
  void initSearch(Game &game, const TimeLimit &timeLimit, bool talking);
  bool isVerbose() {
    return m_verbose;
  }

  ExecutableMove checkForSingleMove();
public:
  AbstractMoveFinder(Player player);
  virtual ~AbstractMoveFinder() {
  }
  Player getPlayer() const {
    return m_player;
  }
  virtual ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool verbose ,bool hint) = 0; // timeout in millisceonds
  virtual String         getName()            const = 0;
  virtual EngineType     getEngineType()      const = 0;
  bool                   isRemote()           const {
    return getEngineType() == REMOTE_ENGINE;
  }
  virtual PositionType   getPositionType()    const = 0;
  virtual void           setVerbose(bool verbose) {
    m_verbose = verbose;
  }
  inline unsigned char getStopCode() const {
    return m_stopCode;
  }
  virtual void stopThinking(bool stopImmediately = true);
  virtual String getStateString(Player computerPlayer, bool detailed) = 0;
  virtual void notifyGameChanged(const Game &game) {
  };
  virtual void notifyMove(const MoveBase &move) {
  }
  virtual bool acceptUndoMove() {
    return true;
  }
  virtual MoveFinderCommand getCommand() {
    return CMD_GETMOVE;
  }
};
