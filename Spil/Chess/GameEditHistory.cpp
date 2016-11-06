#include "stdafx.h"
#include "GameEditHistory.h"

#ifdef _DEBUG
#define CHECKINVARIANT    checkInvariant(__LINE__)
#define TRACESTATE(label) printState(_T(#label))
#define PRINTKEY0()       verbose(_T("key0  :%s\n"), m_key0.toString().replace('\n',_T("")).cstr())
#else
#define CHECKINVARIANT
#define TRACESTATE(label)
#define PRINTKEY0()
#endif

GameEditHistory::GameEditHistory() {
}

Game &GameEditHistory::beginEdit(const Game &game) {
  if(m_game.isSetupMode()) {
    throwException(_T("%s:Alredy in editmode"), __TFUNCTION__);
  }
  resetHistory();
  m_key0 = game.getKey();
  m_game = m_key0;
  m_game.setName(game.getFileName());
  PRINTKEY0();
  TRACESTATE(beginEdit);
  CHECKINVARIANT;

  m_game.beginSetup();
  return m_game;
}

void GameEditHistory::endEdit() {
  if(!m_game.isSetupMode()) {
    throwException(_T("%s:Not in editmode"), __TFUNCTION__);
  }
  m_history.clear();
  m_game.newGame();
  m_game.endSetup();
  m_index = 0;
}

bool GameEditHistory::isModified() {
  CHECKINVARIANT;

  if(m_index == 0) {
    return false;
  }
  m_game.initState();
  return m_game.getKey() != m_key0;
}

Game &GameEditHistory::setGame(const Game &game) {
  CHECKINVARIANT;
  m_game = game;
  CHECKINVARIANT;

  return m_game;
}

Game &GameEditHistory::saveState() {
  CHECKINVARIANT;

  m_game.initState();

  if(canAddKey()) {
    while(canRedo()) {
      removeLast();
    }
    addKey();
    m_index = getHistorySize();
  }

  CHECKINVARIANT;
  TRACESTATE(saveState);

  return m_game;
}

void GameEditHistory::undo(bool all) {
  CHECKINVARIANT;

  if(canUndo()) {
    if(!canRedo()) {
      addKey();
      m_index = all ? 1 : max(getHistorySize() - 1,1);
    }
    setGame(getHistoryKey(--m_index)).beginSetup();
  }

  TRACESTATE(undo);
  CHECKINVARIANT;
}

void GameEditHistory::redo() {
  CHECKINVARIANT;

  if(canRedo()) {
    setGame(getHistoryKey(++m_index)).beginSetup();
  }

  TRACESTATE(redo);
  CHECKINVARIANT;
}

bool GameEditHistory::canUndo() const {
  CHECKINVARIANT;

  return (m_index > 0) && (m_game.getKey() != m_history[m_index-1]);
}

bool GameEditHistory::canRedo() const {
  CHECKINVARIANT;

  return (m_index < getHistorySize() - 1);
}

// private

void GameEditHistory::resetHistory() {
  m_history.clear();
  m_index = 0;
}

const GameKey &GameEditHistory::getHistoryKey(int i) const {
  return i ? m_history[i] : m_key0;
}

bool GameEditHistory::canAddKey() const {
  return m_history.isEmpty() || m_game.getKey() != m_history.last();
}

void GameEditHistory::addKey() {
  if(canAddKey()) {
    m_history.add(m_game.getKey());
  }
}

void GameEditHistory::removeLast() {
  m_history.removeLast();
}

#ifdef _DEBUG
void GameEditHistory::checkInvariant(int line) const {
  if(m_index < 0 || m_index > (int)m_history.size()) {
    verbose(_T("Broken invariant in %s, line %d:index=%d, historySize=%d"),__FILE__,line,m_index,m_history.size());
  }
}


void GameEditHistory::printState(const TCHAR *function) const {
  updateMessageField(_T("%s:history.size:%d, m_index:%d canundo:%s canredo:%s")
         ,function
         ,getHistorySize()
         ,m_index
         ,boolToStr(canUndo())
         ,boolToStr(canRedo())
   );
   verbose(_T("m_game:%s\nlast  :%s\n")
         ,m_game.getKey().toString().replace('\n',_T("")).cstr()
         ,(m_history.size() == 0)?_T("---") : m_history.last().toString().replace('\n',_T("")).cstr()
          );
}

#endif
