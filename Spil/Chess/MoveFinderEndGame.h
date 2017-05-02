#pragma once

#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"

class MoveFinderEndGame : public AbstractMoveFinder, OptionsAccessor {
private:
  EndGameTablebase      *m_tablebase;
  static String          s_currentDbPath;
  static TablebaseMetric s_currentMetric;
public:
  MoveFinderEndGame(Player player, MFTRQueue &msgQueue, EndGameTablebase *tablebase);
  ~MoveFinderEndGame();

  void findBestMove(const FindMoveRequestParam &param, bool talking);
  void stopSearch() {
  }
  void moveNow() {
  }

  String getName() const;
  EngineType getEngineType()  const {
    return INTERN_ENGINE;
  }
  bool isRemote() const {
    return false;
  }
  PositionType getPositionType() const {
    return TABLEBASE_POSITION;
  }

  PositionSignature getPositionSignature() const {
    return m_tablebase->getKeyDefinition().getPositionSignature();
  }

  String getStateString(Player computerPlayer, bool detailed);

};
