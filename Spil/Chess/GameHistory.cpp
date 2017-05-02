#include "stdafx.h"

void Game::resetGameHistory() {
  checkSetupMode(true);

  m_stackSize = 0;

#ifndef TABLEBASE_BUILDER
  memset(m_gameStack, 0, sizeof(m_gameStack[0])*m_stackCapacity);
  m_hashStack.clear();
  m_lastCaptureOrPawnMove = 0;
  m_hashStack.push(m_gameKey);
#endif
}

GameHistory Game::getHistory() const {
  Game tmp = getStartPosition();
  GameHistory result;
  for(UINT i = 0; i < m_stackSize; i++) {
    const PrintableMove m(tmp,getMove(i));
    result.add(m);
    tmp.executeMove(m);
  }
  return result;
}

MoveBase Game::getLastMove() const {
  if(getPlyCount() == 0) {
    return MoveBase().setNoMove();
  } else {
    return getMove(getPlyCount()-1);
  }
}

Move &Game::getMove(UINT ply) { // ply must be in range [0..m_stackSize-1]
  if(ply >= m_stackSize) {
    throwInvalidArgumentException(__TFUNCTION__, _T("ply=%d. plyCount=%d"), ply, m_stackSize);
  }
  return m_gameStack[ply];
}

const Move &Game::getMove(UINT ply) const { // ply must be in range [0..m_stackSize-1]
  if(ply >= m_stackSize) {
    throwInvalidArgumentException(__TFUNCTION__, _T("ply=%d. plyCount=%d"), ply, m_stackSize);
  }
  return m_gameStack[ply];
}

String Game::getMoveString(const MoveBase &m, MoveStringFormat mf) const {
  return PrintableMove(*this, m).toString(mf);
}
