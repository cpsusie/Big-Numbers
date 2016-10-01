#pragma once

#include "AbstractMoveFinder.h"

class MoveFinderRandomPlay : public AbstractMoveFinder {
public:
  MoveFinderRandomPlay(Player player) : AbstractMoveFinder(player) {
  }
  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint);
  String getName() const;

  EngineType getEngineType()  const {
    return RANDOM_ENGINE;
  }

  bool isRemote() const {
    return false;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  String getStateString(Player computerPlayer, bool detailed);
};

