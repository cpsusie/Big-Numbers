#pragma once

#include "AbstractMoveFinder.h"

class MoveFinderNegamax : public AbstractMoveFinder {
private:
  int                 m_bestScore;
  int                 m_evaluationCount;
public:
  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint);

  EngineType getEngineType()  const {
    return INTERN_ENGINE;
  }

  bool isRemote() const {
    return false;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  String getStateString(Player computerPlayer, bool detailed);

};
