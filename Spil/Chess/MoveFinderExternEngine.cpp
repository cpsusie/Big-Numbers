#include "stdafx.h"

#ifndef TABLEBASE_BUILDER

#include "MoveFinderExternEngine.h"

MoveFinderExternEngine::MoveFinderExternEngine(Player player)
: AbstractMoveFinder(player)
, m_externEngine(Options::getEnginePathByPlayer(player))
{
  m_externEngine.start();
  m_externEngine.setParameters(getOptions().getEngineOptionValues(player, getOptions().getPlayerOptions(player).m_engineName));
  m_optionsDlgThread = NULL;
}

MoveFinderExternEngine::~MoveFinderExternEngine() {
  try {
    stopThinking();
    if(m_optionsDlgThread) {
      m_optionsDlgThread->closeThread();
    }
  } catch(...) {
  }
}


void MoveFinderExternEngine::runOptionsDialog() {
  m_optionsDlgThread = CEngineOptionsDlgThread::startThread(this);
}

void MoveFinderExternEngine::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if((source == m_optionsDlgThread) && (id == ENGINEOPIONDIALOG_RUNNING)) {
    const bool isRunning = *(const bool*)newValue;
    if(!isRunning) {
      m_optionsDlgThread = NULL;
    }
  }
}

ExecutableMove MoveFinderExternEngine::findBestMove(Game &game, const TimeLimit &timeLimit, bool talking, bool hint) {
  initSearch(game, timeLimit, talking);
  const ExecutableMove m = checkForSingleMove();
  if(m.isMove()) {
    return m;
  } else {
    return m_externEngine.findBestMove(game, timeLimit, hint);
  }
}

String MoveFinderExternEngine::getName() const {
  return m_externEngine.getDescription().getName();
}

void MoveFinderExternEngine::stopThinking(bool stopImmediately) {
  AbstractMoveFinder::stopThinking(stopImmediately);
  if(stopImmediately) {
    m_externEngine.stopSearch();
  } else {
    m_externEngine.moveNow();
  }
}

void MoveFinderExternEngine::setVerbose(bool verbose) {
  AbstractMoveFinder::setVerbose(verbose);
  m_externEngine.setVerbose(isVerbose());
}

void MoveFinderExternEngine::notifyGameChanged(const Game &game) {
  m_externEngine.notifyGameChanged(game);
}

void MoveFinderExternEngine::notifyMove(const MoveBase &move) {
}

String MoveFinderExternEngine::getStateString(Player computerPlayer, bool detailed) {
  return m_externEngine.toString();
}

#endif // TABLEBASE_BUILDER
