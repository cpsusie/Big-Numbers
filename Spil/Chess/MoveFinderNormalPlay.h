#pragma once

#include "AbstractMoveFinder.h"

class MoveFinderNormalPlay : public AbstractMoveFinder {
private:
  int                 m_maxDepth;
  Move                m_bestMove;
  int                 m_bestScore;
  int                 m_evaluationCount;
  int                 m_sign;

  TCHAR                m_movesString[1024];
  int                 m_movePositions[20];
  int                 m_gameLength0; // #moves for depth = 0
  int                 m_lastGameLength;

  int maximize();
  int maximize(int cMin, int depth);
  int minimize(int cMax, int depth);

  int getLookahead(int level) const;
public:
  ExecutableMove findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint);
  String getName() const;

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

