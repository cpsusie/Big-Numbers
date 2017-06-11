#pragma once

#include "AbstractMoveFinder.h"
#include "SocketChannel.h"

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

class MoveFinderRemotePlayer : public AbstractMoveFinder, public OptionsAccessor {
private:
  SocketChannel  m_channel;
  void              sendCommand(MoveFinderCommand cmd);
  MoveFinderCommand getCommand();
  void              sendMove(const MoveBase &m);
  String            receiveMove();
  void              interrupt();
  void              disConnect();
public:
  MoveFinderRemotePlayer(Player player, ChessPlayerRequestQueue &msgQueue, SocketChannel channel);
  ~MoveFinderRemotePlayer();
  void findBestMove(const RequestParamFindMove &param);
  void stopSearch() {
    interrupt();
  }

  void moveNow() {
  }

  String getName() const {
    return _T("Remote computer");
  }

  MoveFinderType getType()  const {
    return REMOTE_PLAYER;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  void setVerbose(bool verbose) {
  }
  void notifyGameChanged(const Game &game);
  void notifyMove(const PrintableMove &m);

  String getStateString(bool detailed) {
    return _T("remote");
  }

  bool isConnected() const {
    return m_channel.isOpen();
  }
};
