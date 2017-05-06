#pragma once

#include "AbstractMoveFinder.h"

class MoveFinderRandomPlay : public AbstractMoveFinder {
public:
  MoveFinderRandomPlay(Player player, ChessPlayerRequestQueue &msgQueue) : AbstractMoveFinder(player, msgQueue) {
  }
  void findBestMove(const FindMoveRequestParam &param);
  void stopSearch() {
  }
  void moveNow() {
  }
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

