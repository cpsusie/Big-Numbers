#include "stdafx.h"

#if !defined(TABLEBASE_BUILDER)

#include "MoveFinderExternEngine.h"

MoveFinderExternEngine::MoveFinderExternEngine(Player player, ChessPlayerRequestQueue &msgQueue)
: AbstractMoveFinder(player, msgQueue)
, m_externEngine(player, Options::getEnginePathByPlayer(player))
{
  m_externEngine.start(this);
  m_externEngine.setParameters(getOptions().getEngineOptionValues(player, getOptions().getPlayerOptions(player).m_engineName));
  m_optionsDlgThread = nullptr;
}

MoveFinderExternEngine::~MoveFinderExternEngine() {
  try {
    m_externEngine.quit();
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
      m_optionsDlgThread = nullptr;
    }
  }
}

void MoveFinderExternEngine::findBestMove(const RequestParamFindMove &param) {
  initSearch(param);
  const PrintableMove m = checkForSingleMove();
  if(m.isMove()) {
    putMove(m);
  } else {
    if(!m_externEngine.isReady() || !m_externEngine.isThreadRunning()) {
      throwException(_T("Extern engine is dead:Flags:%s"), m_externEngine.flagsToString().cstr());
    }
    m_externEngine.findBestMove(param.getGame(), param.getTimeLimit());
  }
}

String MoveFinderExternEngine::getName() const {
  return m_externEngine.getDescription().getName();
}

void MoveFinderExternEngine::stopSearch() {
  m_externEngine.stopSearch();
}

void MoveFinderExternEngine::moveNow() {
  m_externEngine.moveNow();
}

void MoveFinderExternEngine::setVerbose(bool verbose) {
  AbstractMoveFinder::setVerbose(verbose);
  m_externEngine.setVerbose(isVerbose());
}

void MoveFinderExternEngine::notifyGameChanged(const Game &game) {
  m_externEngine.notifyGameChanged(game);
}

String MoveFinderExternEngine::getStateString(bool detailed) {
  return m_externEngine.toString();
}

#endif // TABLEBASE_BUILDER
