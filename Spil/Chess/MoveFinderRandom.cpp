#include "stdafx.h"
#include "MoveFinderRandom.h"

void MoveFinderRandomPlay::findBestMove(const FindMoveRequestParam &param) {
  initSearch(param);
  randomize();
  putMove(m_game.getRandomMove());
}

String MoveFinderRandomPlay::getName() const {
  return _T("Random play");
}

String MoveFinderRandomPlay::getStateString(bool detailed) {
  return _T("No state for MoveFinderRandomPlay");
}

