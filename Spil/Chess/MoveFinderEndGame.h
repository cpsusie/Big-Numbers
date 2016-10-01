#pragma once

#include "AbstractMoveFinder.h"
#include "EndGameTablebase.h"

class MoveFinderEndGame : public AbstractMoveFinder, OptionsAccessor {
private:
  EndGameTablebase      *m_tablebase;
  static String          m_currentDbPath;
  static TablebaseMetric m_currentMetric;
public:
  MoveFinderEndGame(Player player, EndGameTablebase *tablebase);
  ~MoveFinderEndGame();

  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint); // timeout millisceonds

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
