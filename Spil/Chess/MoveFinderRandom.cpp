#include "stdafx.h"
#include "MoveFinderRandom.h"

ExecutableMove MoveFinderRandomPlay::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);
  randomize();
  const Move m = game.getRandomMove();
  return ExecutableMove(game, m);
}

String MoveFinderRandomPlay::getName() const {
  return _T("Random play");
}

String MoveFinderRandomPlay::getStateString(Player computerPlayer, bool detailed) {
  return _T("No state for MoveFinderRandomPlay\n");
}

