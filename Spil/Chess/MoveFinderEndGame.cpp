#include "stdafx.h"
#include "MoveFinderEndGame.h"

//#define FIND_ENDMOVE_BY_SEARCH

String          MoveFinderEndGame::m_currentDbPath;
TablebaseMetric MoveFinderEndGame::m_currentMetric;

MoveFinderEndGame::MoveFinderEndGame(Player player, EndGameTablebase *tablebase) 
: AbstractMoveFinder(player)
, m_tablebase(tablebase)
{
}

MoveFinderEndGame::~MoveFinderEndGame() {
  m_tablebase->unload();
}

ExecutableMove MoveFinderEndGame::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);

  if(!m_tablebase->isLoaded() 
   || (m_currentDbPath != EndGameKeyDefinition::getDbPath()) 
   || (m_currentMetric != EndGameKeyDefinition::getMetric())) {
    if(m_tablebase->isLoaded()) {
      m_tablebase->unload();
    }
    m_currentDbPath = EndGameKeyDefinition::getDbPath();
    m_currentMetric = EndGameKeyDefinition::getMetric();
//    verbose(_T("Loading tablebase %s. Metric:%s..."), m_tablebase->getName().cstr(), EndGameKeyDefinition::getMetricName(m_currentMetric));
    m_tablebase->load();
  } else {
    if(isVerbose()) {
      verbose(_T("Tablebase %s. Metric:%s\n"), m_tablebase->getName().cstr(), EndGameKeyDefinition::getMetricName(m_currentMetric));
    }
  }
  MoveResultArray allMoves(game.getPlayerInTurn());
  const Options &options = getOptions();
  ExecutableMove result = m_tablebase->findBestMove(game, allMoves, options.getEndGameDefendStrength());
  if(result.isMove()) {
    if(isVerbose()) {
      verbose(_T("%s"), allMoves.toString(game, options.getMoveFormat(), options.getDepthInPlies()).cstr());
    }
  } else {
    switch(game.findGameResult()) {
    case NORESULT                          :
      throwException(_T("MoveFinder(%s)::findBestMove:Cannot find best move in position [%s]")
                    ,m_tablebase->getPositionTypeString().cstr()
                    ,m_tablebase->toString(game).cstr());

    case WHITE_CHECKMATE           :
    case BLACK_CHECKMATE           :
    case STALEMATE                 :
#ifndef TABLEBASE_BUILDER
    case POSITION_REPEATED_3_TIMES :
    case NO_CAPTURE_OR_PAWNMOVE    :
    case DRAW                      :
#endif
      break;
    }
  }
  return result;
}

String MoveFinderEndGame::getName() const {
  return format(_T("Tablebase %s"), m_tablebase->getName().cstr());
}

String MoveFinderEndGame::getStateString(Player computerPlayer, bool detailed) {
  String result;
  if(m_tablebase) {
    result = format(_T("Current tablebase:%s\n"), m_tablebase->getName().cstr());
    if(m_tablebase->isLoaded()) {
      result += _T("  Loaded\n");
    } else {
      result += _T("  Not loaded\n");
    }
    result += format(_T("  LoadRefCount:%d\n"), m_tablebase->getLoadRefCount());
  }
  result += format(_T("  Current metric:%s\n"), EndGameKeyDefinition::getMetricName());
  const EndGameTablebaseList list = EndGameTablebase::getExistingEndGameTablebases();
  String allLoadedString;
  for(size_t i = 0; i < list.size(); i++) {
    const EndGameTablebase *tb = list[i];
    if(tb->isLoaded()) {
      allLoadedString += format(_T("%5s refcount:%d\n"), tb->getName().cstr(), tb->getLoadRefCount());
    }
  }
  if(allLoadedString.length() > 0) {
    result += format(_T("All loaded tablebases:\n%s"), allLoadedString.cstr());
  }
  return result;
}
