#include "stdafx.h"
#include "GameEditHistory.h"

#ifdef _DEBUG
#define CHECKINVARIANT(onEnter) checkInvariant(__TFUNCTION__,onEnter)
#define TRACESTATE()            printState(__TFUNCTION__)
#define ENTER true
#define LEAVE false
#else
#define CHECKINVARIANT(onEnter)
#define TRACESTATE()
#endif // _DEBUG

GameEditHistory::GameEditHistory() {
  resetHistory();
}

Game &GameEditHistory::beginEdit(const Game &game) {
  if(m_game.isSetupMode()) {
    throwException(_T("%s:Alredy in editmode"), __TFUNCTION__);
  }
  resetHistory();
  m_history.add(game.getKey());
  m_game = getKey(0);
  m_game.setName(game.getFileName());
  m_game.beginSetup();
  TRACESTATE();
  CHECKINVARIANT(LEAVE);
  return m_game;
}

void GameEditHistory::endEdit() {
  if(!m_game.isSetupMode()) {
    throwException(_T("%s:Not in editmode"), __TFUNCTION__);
  }
  m_game.newGame();
  m_game.endSetup();
  resetHistory();
}

bool GameEditHistory::isModified() {
  CHECKINVARIANT(ENTER);
  m_game.initState();
  return m_game.getKey() != getKey(0);
}

Game &GameEditHistory::setGame(const Game &game) {
  CHECKINVARIANT(ENTER);
  m_game = game;
  CHECKINVARIANT(LEAVE);
  return m_game;
}

Game &GameEditHistory::saveState() {
  CHECKINVARIANT(ENTER);
  m_game.initState();
  while(canRedo()) {
    removeLast();
  }
  if(canAddKey()) {
    addKey();
    m_index = getHistorySize()-1;
  }
  TRACESTATE();
  CHECKINVARIANT(LEAVE);
  return m_game;
}

void GameEditHistory::undo(bool all) {
  CHECKINVARIANT(ENTER);
  if(canUndo()) {
    const size_t newIndex = all ? 0 : (m_index-1);
    if(!canRedo() && canAddKey()) {
      addKey();
    }
    setGame(getKey(m_index=newIndex)).beginSetup();
  }
  TRACESTATE();
  CHECKINVARIANT(LEAVE);
}

void GameEditHistory::redo() {
  CHECKINVARIANT(ENTER);
  if(canRedo()) {
    setGame(getKey(++m_index)).beginSetup();
  }
  TRACESTATE();
  CHECKINVARIANT(LEAVE);
}

bool GameEditHistory::canUndo() const {
  CHECKINVARIANT(ENTER);
  return m_index > 0;
}

bool GameEditHistory::canRedo() const {
  CHECKINVARIANT(ENTER);
  return !isHistoryEmpty() && (m_index < getHistorySize() - 1);
}

// ------------------------------------------ private ------------------------------------------------

#ifdef _DEBUG
  // Invariant: (0 < m_historySize()) && (0 <= m_index < m_history.size())
  //        &&  m_history[i-1] != m_history[i], i = [1..m_history.size()-1]
  // index < m_history.size()-1 => m_game.key() == m_history[m_index]
void GameEditHistory::checkInvariant(const TCHAR *method, bool enter) const {
  if((m_index < 0) || (m_index >= m_history.size())
   || (!enter && (m_index < m_history.size()-1) && (m_game.getKey() != getKey(m_index)))
    ) {
    showWarning(_T("Broken invariant in %s, %s:index=%zu, historySize=%zu\nm_game.key;%s\ngetKey(%zu);%s")
               ,method, enter?_T("Enter"):_T("Leave")
               ,m_index,m_history.size()
               ,m_game.getKey().toFENString().cstr()
               ,m_index,getKey(m_index).toFENString().cstr()
               );
  }
  const size_t n = m_history.size();
  for(size_t i = 1; i < n; i++) {
    if(getKey(i-1) == getKey(i)) {
      showWarning(_T("Broken invariant in %s, %s:key[%zu] == key[%zu]")
                 ,method, enter?_T("Enter"):_T("Leave")
                 ,i-1, i);
      break;
    }
  }
}

void GameEditHistory::printState(const TCHAR *method) const {
  clearVerbose();
  updateMessageField(0
                    ,_T("%s:history.size:%zu, index:%zu canundo:%s canredo:%s")
                    ,method
                    ,getHistorySize()
                    ,m_index
                    ,boolToStr(canUndo())
                    ,boolToStr(canRedo())
   );

   const size_t n = m_history.size();
   for(size_t i = 0; i < n; i++) {
     verbose(_T("his[%2zu] :%s%s"), i, m_history[i].toFENString().cstr()
            ,(i==m_index) ? _T("<---\n") : _T("\n"));
   }
   verbose(_T("game    :%s\n"), m_game.getKey().toFENString().cstr());
}

#endif // _DEBUG
