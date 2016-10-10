#pragma once

#include "AbstractMoveFinder.h"
#include "SocketChannel.h"

class MoveFinderRemotePlayer : public AbstractMoveFinder, public OptionsAccessor {
private:
  SocketChannel  m_channel;
  MoveBase       m_receivedMove;
  void              sendCommand(MoveFinderCommand cmd);
  MoveFinderCommand getCommand();
  void              sendMove(const MoveBase &m);
  String            receiveMove();
  void              interrupt();
  void              disConnect();
public:
  MoveFinderRemotePlayer(Player player, SocketChannel channel);
  ~MoveFinderRemotePlayer();
  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint);
  String getName() const;

  EngineType getEngineType()  const {
    return REMOTE_ENGINE;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  void stopThinking(bool stopImmediately = true);
  void setVerbose(bool verbose);
  void notifyGameChanged(const Game &game);
  void notifyMove(const MoveBase &move);

  String getStateString(Player computerPlayer, bool detailed);

  bool isConnected() const {
    return m_channel.isOpen();
  }
};
