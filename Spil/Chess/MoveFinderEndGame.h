#pragma once

#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"

class MoveFinderEndGame : public AbstractMoveFinder, OptionsAccessor {
private:
  EndGameTablebase      *m_tablebase;
  static String          s_currentDbPath;
  static TablebaseMetric s_currentMetric;
public:
  MoveFinderEndGame(Player player, ChessPlayerRequestQueue &msgQueue, EndGameTablebase *tablebase);
  ~MoveFinderEndGame();

  void findBestMove(const FindMoveRequestParam &param);
  void stopSearch() {
  }
  void moveNow() {
  }

  String getName() const;
  MoveFinderType getType()  const {
    return TABLEBASE_PLAYER;
  }
  PositionType getPositionType() const {
    return TABLEBASE_POSITION;
  }

  PositionSignature getPositionSignature() const {
    return m_tablebase->getKeyDefinition().getPositionSignature();
  }

  String getStateString(bool detailed);

};
