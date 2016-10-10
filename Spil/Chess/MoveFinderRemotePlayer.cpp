#include "stdafx.h"
#include "MoveFinderRemotePlayer.h"

MoveFinderRemotePlayer::MoveFinderRemotePlayer(Player player, SocketChannel channel)
: AbstractMoveFinder(player)
, m_channel(channel)
{
  m_receivedMove.setNoMove();
}

void MoveFinderRemotePlayer::disConnect() {
  m_channel.close();
}

MoveFinderRemotePlayer::~MoveFinderRemotePlayer() {
  try {
    stopThinking();
    disConnect();
  } catch(...) {
  }
}

void MoveFinderRemotePlayer::interrupt() {
  sendCommand(CMD_INTERRUPT);
}

void MoveFinderRemotePlayer::sendCommand(MoveFinderCommand cmd) {
  m_channel.writeInt(cmd);
}

MoveFinderCommand MoveFinderRemotePlayer::getCommand() {
  try {
    return (MoveFinderCommand)m_channel.readInt();
  } catch(...) {
    disConnect();
    throw;
  }
}

void MoveFinderRemotePlayer::sendMove(const MoveBase &m) {
  try {
    sendCommand(CMD_GETMOVE);
    m_channel.write(m.toString());
  } catch(...) {
    disConnect();
    throw;
  }
}

String MoveFinderRemotePlayer::receiveMove() {
  try {
    const MoveFinderCommand cmd = getCommand();
    if (cmd != CMD_GETMOVE) {
      throw cmd;
    }
    String s;
    return m_channel.read(s);
  } catch(...) {
    disConnect();
    throw;
  }
}

ExecutableMove MoveFinderRemotePlayer::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);
  const String s = receiveMove();
  ExecutableMove result;
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
    sendMove(move);
  }
}

String MoveFinderRemotePlayer::getStateString(Player computerPlayer, bool detailed) {
  return _T("remote");
}
