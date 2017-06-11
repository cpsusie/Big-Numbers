#pragma once

#include "AbstractMoveFinder.h"

class MoveFinderRandomPlay : public AbstractMoveFinder {
public:
  MoveFinderRandomPlay(Player player, ChessPlayerRequestQueue &msgQueue) : AbstractMoveFinder(player, msgQueue) {
  }
  void findBestMove(const RequestParamFindMove &param);
  void stopSearch() {
  }
  void moveNow() {
  }
  String getName() const;

  MoveFinderType getType()  const {
    return RANDOM_PLAYER;
  }

  PositionType getPositionType() const {
    return NORMAL_POSITION;
  }

  String getStateString(bool detailed);
};

