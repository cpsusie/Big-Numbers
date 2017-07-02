#include "stdafx.h"
#include <Exception.h>
#include "GameTypes.h"

void GameHistory::clear() {
  m_gameCount = 0;
  memset(m_scoreList,0,sizeof(m_scoreList));
  for(int i = 0; i < 3; i++) {
    m_playedGames[i].clear();
  }
}

void GameHistory::addResult(const GameDescription &gameDesc, const TrickCount &trickCount) {
  if(m_gameCount >= 12) {
    throwException(_T("Kan ikke tilføje flere runder til gameHistory. count=%d"), m_gameCount);
  }

  if(trickCount.getTotalTrickCount() != 13) {
    throwException(_T("%s:Illegal trickCount. Total TrickCount=%d. Must be 13"), __TFUNCTION__, trickCount.getTotalTrickCount());
  }

  int score[3];
  int p;

  switch(gameDesc.getGameType()) {
  case GAMETYPE_SOL:
    for(p = 0; p < 3; p++) {
      score[p] = (gameDesc.getStandardTrickCount(p) - trickCount.getTrickCount(p)) * gameDesc.getPointsPerTrick();
    }
    break;

  case GAMETYPE_SANS:
  case GAMETYPE_SPAR:
  case GAMETYPE_FARVE:
    for(p = 0; p < 3; p++) {
      score[p] = (trickCount.getTrickCount(p) - gameDesc.getStandardTrickCount(p)) * gameDesc.getPointsPerTrick();
    }
    break;
  }

  for(p = 0; p < 3; p++) {
    m_scoreList[m_gameCount][p] = (m_gameCount > 0) ? m_scoreList[m_gameCount-1][p] : 0;
  }

  for(p = 0; p < 3; p++) {
    m_scoreList[m_gameCount][p] += score[p];
  }
  m_gameCount++;
}

void GameHistory::addPlayedGame(const GameDescription &gameDesc) {
  int      player0        = gameDesc.getPlayer0();
  GameType gameType       = gameDesc.getGameType();
  int      pointsPerTrick = gameDesc.getPointsPerTrick();

  if(player0 < 0 || player0 > 2) {
    throwException(_T("%s:Illegal player0=%d. Must be [0..2]"), __TFUNCTION__, player0);
  }
  m_playedGames[player0].addPlayedGame(gameType,pointsPerTrick);
}

bool GameHistory::isPointsUsed(UINT playerId, int pointsPerTrick) const {
  if(playerId > 2) {
    throwException(_T("%s:Illegal playerId=%d. Must be [0..2]"), __TFUNCTION__, playerId);
  }
  return m_playedGames[playerId].isPointsUsed(pointsPerTrick);
}

bool GameHistory::isGameUsed(UINT playerId, GameType gameType) const {
  if(playerId > 2) {
    throwException(_T("%s:Invalid playerId=%d. Must be [0..2]"), __TFUNCTION__, playerId);
  }
  return m_playedGames[playerId].isGameUsed(gameType);
}

void GamesPlayed::clear() {
  memset(m_playedGames,0,sizeof(m_playedGames));
}

int GamesPlayed::getCount() const {
  int count = 0;
  for(int i = 0; i < 4; i++) {
    if(m_playedGames[i] != 0) {
      count++;
    }
  }
  return count;
}

void GamesPlayed::setRandom(UINT count) {
  if(count > 4) {
    throwInvalidArgumentException(__TFUNCTION__,_T("count=%d. Must be [0..4]"),count);
  }
  clear();
  while(getCount() < (int)count) {
    GameType gameType;
    for(;;) {
      const int t = rand() % 4;
      if(!isGameUsed((GameType)t)) {
        gameType = (GameType)t;
        break;
      }
    }
    int pointsPerTrick;
    for(;;) {
      const int p = rand() % 4 + 1;
      if(!isPointsUsed(p)) {
        pointsPerTrick = p;
        break;
      }
    }
    addPlayedGame(gameType,pointsPerTrick);
  }
}

void GamesPlayed::addPlayedGame(GameType gameType, int pointsPerTrick) {
  DEFINEMETHODNAME;
  if(pointsPerTrick < 1 || pointsPerTrick > 4) {
    throwInvalidArgumentException(method, _T("pointsPerTrick=%d. Must be [1..4]"), pointsPerTrick);
  }
  if(m_playedGames[gameType] != 0) {
    throwInvalidArgumentException(method, _T("Valgt spil %s til %d point er allerede valgt en gang"), gameTypeString[gameType], pointsPerTrick);
  }
  if(isPointsUsed(pointsPerTrick)) {
    throwInvalidArgumentException(method, _T("Valgt antal point %d allerede valgt en gang"), pointsPerTrick);
  }
  m_playedGames[gameType] = pointsPerTrick;
}

bool GamesPlayed::isPointsUsed(int pointsPerTrick) const {
  DEFINEMETHODNAME;
  if(pointsPerTrick < 1 || pointsPerTrick > 4) {
    throwInvalidArgumentException(method, _T("pointsPerTrick=%d. Must be [1..4]"), pointsPerTrick);
  }
  for(int i = 0; i < 4; i++) {
    if(m_playedGames[i] == pointsPerTrick) {
      return true;
    }
  }
  return false;
}

bool GamesPlayed::isGameUsed(  GameType gameType) const {
  return m_playedGames[gameType] != 0;
}

static String pstr(int n) {
  return (n == 0) ? _T(" ") : format(_T("%d"),n);
}

String GameHistory::toString(UINT myId, GameType gameType) const {
  DEFINEMETHODNAME;
  if(myId < 0 || myId > 2) {
    throwInvalidArgumentException(method, _T("playerId=%d. Must be [0..2]"), myId);
  }
  return format(_T("%-*s %*s %*s %*s")
               ,LEFTFIELDWIDTH ,gameTypeString[gameType]
               ,FIELDWIDTH,pstr(m_playedGames[MOD3(myId+0)].getPointsPerTrick(gameType)).cstr()
               ,FIELDWIDTH,pstr(m_playedGames[MOD3(myId+1)].getPointsPerTrick(gameType)).cstr()
               ,FIELDWIDTH,pstr(m_playedGames[MOD3(myId+2)].getPointsPerTrick(gameType)).cstr());
};

String GameHistory::toString(UINT myId, int index) const {
  DEFINEMETHODNAME;
  if(myId < 0 || myId > 2) {
    throwInvalidArgumentException(method, _T("playerId=%d. Must be [0..2]"), myId);
  }
  if(index < 0 || index >= m_gameCount) {
    throwInvalidArgumentException(method, _T("Index=%d out of range. gameCount=%d"),index, m_gameCount);
  }
  return format(_T("%-*d %*d %*d %*d")
               ,LEFTFIELDWIDTH ,index+1
               ,FIELDWIDTH,m_scoreList[index][MOD3(myId+0)]
               ,FIELDWIDTH,m_scoreList[index][MOD3(myId+1)]
               ,FIELDWIDTH,m_scoreList[index][MOD3(myId+2)]);
}
