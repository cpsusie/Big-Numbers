#include "stdafx.h"
#include "MoveFinderRemotePlayer.h"

MoveFinderRemotePlayer::MoveFinderRemotePlayer(Player player, ChessPlayerRequestQueue &msgQueue, SocketChannel channel)
: AbstractMoveFinder(player, msgQueue)
, m_channel(channel)
{
  m_receivedMove.setNoMove();
}

void MoveFinderRemotePlayer::disConnect() {
  m_channel.close();
}

MoveFinderRemotePlayer::~MoveFinderRemotePlayer() {
  try {
    stopSearch();
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

void MoveFinderRemotePlayer::findBestMove(const FindMoveRequestParam &param, bool talking) {
  initSearch(param, talking);
  const String s = receiveMove();
  PrintableMove result;
  if(s.length() == 0) {
    m_receivedMove = result.setNoMove();
  } else {
    result = m_game.generateMove(s);
    m_receivedMove = result;
  }
  return putMove(result);
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
