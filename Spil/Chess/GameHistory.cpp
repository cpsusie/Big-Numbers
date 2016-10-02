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
    const Move          &m1 = getMove(i);
    const ExecutableMove m2 = tmp.generateMove(m1.getFrom(), m1.getTo(), m1.getPromoteTo(), m1.getAnnotation());
    result.add(m2);
    tmp.executeMove(m2);
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
    throwInvalidArgumentException(_T("getMove"), _T("ply=%d. plyCount=%d"), ply, m_stackSize);
  }
  return m_gameStack[ply];
}

const Move &Game::getMove(UINT ply) const { // ply must be in range [0..m_stackSize-1]
  if(ply >= m_stackSize) {
    throwInvalidArgumentException(_T("getMove"), _T("ply=%d. plyCount=%d"), ply, m_stackSize);
  }
  return m_gameStack[ply];
}

String Game::getMoveString(const Move &m, MoveStringFormat mf) const {
  return ExecutableMove(*this, m).toString(mf);
}
