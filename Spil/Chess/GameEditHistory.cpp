#include "stdafx.h"
#include "GameEditHistory.h"

#if defined(_DEBUG)
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
    const int newIndex = all ? 0 : (m_index-1);
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

String GameEditHistory::getFen(int index) const {
  if((index >= 0) && (index < getHistorySize())) {
    return getKey(index).toFENString();
  } else {
    return format(_T("fen[%d] = out of range"), index);
  }
}

#if defined(_DEBUG)
  // Invariant: (0 < m_historySize()) && (0 <= m_index < m_history.size())
  //        &&  m_history[i-1] != m_history[i], i = [1..m_history.size()-1]
  // index < m_history.size()-1 => m_game.key() == m_history[m_index]
void GameEditHistory::checkInvariant(const TCHAR *method, bool enter) const {
  const int n = getHistorySize();
  if(((m_index < 0) || m_index >= n) || (!enter && (m_index < n-1) && (m_game.getKey() != getKey(m_index))) ) {
    showWarning(_T("Broken invariant in %s, %s:index=%d, historySize=%d\nm_game.key;%s\ngetKey(%d);%s")
               ,method, enter?_T("Enter"):_T("Leave")
               ,m_index,n
               ,m_game.getKey().toFENString().cstr()
               ,m_index,getFen(m_index).cstr()
               );
  }
  for(int i = 1; i < n; i++) {
    if(getKey(i-1) == getKey(i)) {
      showWarning(_T("Broken invariant in %s, %s:key[%d] == key[%d]"),method, enter?_T("Enter"):_T("Leave"),i-1, i);
      break;
    }
  }
}

void GameEditHistory::printState(const TCHAR *method) const {
  clearVerbose();
  updateMessageField(0
                    ,_T("%s:history.size:%d, index:%d canundo:%s canredo:%s")
                    ,method
                    ,getHistorySize()
                    ,m_index
                    ,boolToStr(canUndo())
                    ,boolToStr(canRedo())
   );

  const int n = getHistorySize();
   for(int i = 0; i < n; i++) {
     verbose(_T("his[%2d] :%s%s"), i, m_history[i].toFENString().cstr(), (i==m_index) ? _T("<---\n") : _T("\n"));
   }
   verbose(_T("game    :%s\n"), m_game.getKey().toFENString().cstr());
}

#endif // _DEBUG
