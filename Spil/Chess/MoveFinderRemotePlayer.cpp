#include "stdafx.h"
#include "MoveFinderRemotePlayer.h"

#define PORTNUMBER 3572

MoveFinderRemotePlayer::MoveFinderRemotePlayer(Player player, SocketChannel channel)
: AbstractMoveFinder(player)
, m_channel(channel)
, m_signalChannel(PORTNUMBER) {
  m_receivedMove.setNoMove();
}

void MoveFinderRemotePlayer::disConnect() {
  m_channel.close();
  m_signalChannel.close();
}

MoveFinderRemotePlayer::~MoveFinderRemotePlayer() {
  try {
    stopThinking();
    disConnect();
  } catch(...) {
  }
}

void MoveFinderRemotePlayer::sendCommand(MoveFinderCommand cmd) {
  m_channel.writeInt(cmd);
}

void MoveFinderRemotePlayer::interrupt() {
  m_signalChannel.writeInt(CMD_INTERRUPT);
}

MoveFinderCommand MoveFinderRemotePlayer::getCommand() {
  try {
    fd_set socketSet;
    FD_ZERO(&socketSet);
    const SOCKET &netSocket = m_channel.getReadSocket();
    const SOCKET &sigSocket = m_signalChannel.getReadSocket();
    FD_SET(netSocket, &socketSet);
    FD_SET(sigSocket, &socketSet);
    if(select(2, &socketSet, NULL, NULL, NULL) == SOCKET_ERROR) {
      throw TcpException(getWSAErrorText(WSAGetLastError()));
    }
    if(FD_ISSET(sigSocket, &socketSet)) {
      return (MoveFinderCommand)m_signalChannel.readInt();
    }
    if(FD_ISSET(netSocket, &socketSet)) {
      return (MoveFinderCommand)m_channel.readInt();
    }
    throwException(_T("select socket returned without any dat aready!"));
    return CMD_INTERRUPT;
  } catch(...) {
    disConnect();
    throw;
  }
}

void MoveFinderRemotePlayer::sendMove(const MoveBase &m) {
  try {
    m_channel.write(m.toString());
  } catch(...) {
    disConnect();
    throw;
  }
}

String MoveFinderRemotePlayer::receiveMove() {
  try {
    String s;
    return m_channel.read(s);
  } catch(...) {
    disConnect();
    throw;
  }
}

ExecutableMove MoveFinderRemotePlayer::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);
  ExecutableMove result;
  const String s = receiveMove();
  if(s.length() == 0) {
    m_receivedMove = result.setNoMove();
  } else {
    result = game.generateMove(s);
    m_receivedMove = result;
  }
  return result;
}

String MoveFinderRemotePlayer::getName() const {
  return _T("Remote computer");
}

void MoveFinderRemotePlayer::stopThinking(bool stopImmediately) {
  AbstractMoveFinder::stopThinking(stopImmediately);
  if(stopImmediately) {
    interrupt();
  }
}

void MoveFinderRemotePlayer::setVerbose(bool verbose) {
}

void MoveFinderRemotePlayer::notifyGameChanged(const Game &game) {
  sendCommand(CMD_GETGAME);
  m_channel.write(game);
  m_receivedMove.setNoMove();
}

void MoveFinderRemotePlayer::notifyMove(const MoveBase &move) {
  if(move != m_receivedMove) {
    sendCommand(CMD_GETMOVE);
    sendMove(move);
  }
}

String MoveFinderRemotePlayer::getStateString(Player computerPlayer, bool detailed) {
  return _T("remote");
}
