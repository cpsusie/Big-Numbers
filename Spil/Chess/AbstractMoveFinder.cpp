#include "stdafx.h"
#include "AbstractMoveFinder.h"
#include "TraceDlgThread.h"

AbstractMoveFinder::AbstractMoveFinder(Player player) : m_player(player) {
  m_stopCode = 0;
  m_verbose  = false;
}

void AbstractMoveFinder::initSearch(Game &game, const TimeLimit &timeLimit, bool talking) {
  m_game = &game;
#ifndef TABLEBASE_BUILDER
  m_game->setMaxPositionRepeat(1);
#endif
  m_timeLimit  = timeLimit;
  m_stopCode   = 0;
  setVerbose(talking);
}

void AbstractMoveFinder::stopThinking(bool stopImmediately) {
  if(stopImmediately) {
    m_stopCode = STOPPED_BY_USER | STOP_IMMEDIATELY;
  } else {
    m_stopCode = STOPPED_BY_USER | STOP_WHENMOVEFOUND;
  }
}

ExecutableMove AbstractMoveFinder::checkForSingleMove() {
  if(m_game->isSingleMovePosition()) { // dont waste time to think. just return the only possible move
    const ExecutableMove result = ExecutableMove(*m_game, m_game->getRandomMove());
    if(isVerbose()) {
      verbose(format(_T("%s:%s\n"), loadString(IDS_MSG_ONLY_1_MOVE).cstr(), result.toString().cstr()).cstr());
    }
    return result;
  }
  return ExecutableMove();
}
