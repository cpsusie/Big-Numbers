#include "stdafx.h"
#include "MoveFinderEndGame.h"

//#define FIND_ENDMOVE_BY_SEARCH

String          MoveFinderEndGame::s_currentDbPath;
TablebaseMetric MoveFinderEndGame::s_currentMetric;

MoveFinderEndGame::MoveFinderEndGame(Player player, ChessPlayerRequestQueue &msgQueue, EndGameTablebase *tablebase)
: AbstractMoveFinder(player, msgQueue)
, m_tablebase(tablebase)
{
}

MoveFinderEndGame::~MoveFinderEndGame() {
  m_tablebase->unload();
}

void MoveFinderEndGame::findBestMove(const RequestParamFindMove &param) {
  initSearch(param);

  if(!m_tablebase->isLoaded()
   || (s_currentDbPath != EndGameKeyDefinition::getDbPath())
   || (s_currentMetric != EndGameKeyDefinition::getMetric())) {
    if(m_tablebase->isLoaded()) {
      m_tablebase->unload();
    }
    s_currentDbPath = EndGameKeyDefinition::getDbPath();
    s_currentMetric = EndGameKeyDefinition::getMetric();
//    verbose(_T("Loading tablebase %s. Metric:%s..."), m_tablebase->getName().cstr(), EndGameKeyDefinition::getMetricName(s_currentMetric));
    m_tablebase->load();
  } else {
    if(isVerbose()) {
      verbose(_T("Tablebase %s. Metric:%s\n"), m_tablebase->getName().cstr(), EndGameKeyDefinition::getMetricName(s_currentMetric));
    }
  }
  MoveResult2Array allMoves;
  const Options &options = getOptions();
  PrintableMove  result  = m_tablebase->findBestMove(m_game, allMoves, options.getEndGameDefendStrength());
  if(result.isMove()) {
    if(isVerbose()) {
      verbose(_T("%s"), allMoves.toString(m_game, options.getMoveFormat(), options.getDepthInPlies()).cstr());
    }
  } else {
    switch(m_game.findGameResult()) {
    case NORESULT                          :
      throwException(_T("%s(%s):Cannot find best move in position [%s]")
                    ,__TFUNCTION__
                    ,m_tablebase->getPositionTypeString().cstr()
                    ,m_tablebase->toString(m_game).cstr());

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
  putMove(result);
}

String MoveFinderEndGame::getName() const {
  return format(_T("Tablebase %s"), m_tablebase->getName().cstr());
}

String MoveFinderEndGame::getStateString(bool detailed) {
  String result;
  if(m_tablebase) {
    result = format(_T("Current tablebase:%s"), m_tablebase->getName().cstr());
    if(m_tablebase->isLoaded()) {
      result += _T("\n  Loaded");
    } else {
      result += _T("\n  Not loaded");
    }
    result += format(_T("\n  LoadRefCount:%d"), m_tablebase->getLoadRefCount());
  }
  result += format(_T("\n  Current metric:%s"), EndGameKeyDefinition::getMetricName());
  const EndGameTablebaseList list = EndGameTablebase::getExistingEndGameTablebases();
  String allLoadedString;
  for(size_t i = 0; i < list.size(); i++) {
    const EndGameTablebase *tb = list[i];
    if(tb->isLoaded()) {
      allLoadedString += format(_T("\n%5s refcount:%d"), tb->getName().cstr(), tb->getLoadRefCount());
    }
  }
  if(allLoadedString.length() > 0) {
    result += format(_T("\nAll loaded tablebases:\n%s"), allLoadedString.cstr());
  }
  return result;
}
