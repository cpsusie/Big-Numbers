#pragma once

class GameEditHistory {
private:
  Game                  m_game;
  CompactArray<GameKey> m_history;
  size_t                m_index;
  // Invariant: (0 < m_historySize()) && (0 <= m_index < m_history.size())
  //        &&  m_history[i-1] != m_history[i], i = [1..m_history.size()-1]
  // index < m_history.size()-1 => m_game.key() == m_history[m_index]

  inline void resetHistory() {
    m_history.clear();
    m_index = 0;
  }
  inline size_t getHistorySize() const {
    return m_history.size();
  }
  inline bool isHistoryEmpty() const {
    return m_history.isEmpty();
  }
  inline void removeLast() {
    m_history.removeLast();
  }
  inline void addKey() {
    m_history.add(m_game.getKey());
  }
  inline bool canAddKey() const {
    return m_game.getKey() != getLastKey();
  }
  // 0 <= i < m_history.size(). return
  inline const GameKey &getKey(size_t i) const {
    return m_history[i];
  }
  inline const GameKey &getLastKey() const {
    return m_history.last();
  }
  Game &setGame(const Game &game);

#if defined(_DEBUG)
  void checkInvariant(const TCHAR *method, bool enter) const;
#endif

public:
  GameEditHistory();

  Game &beginEdit(const Game &game);
  void endEdit();

  inline Game &getGame() {
    return m_game;
  }

  bool isModified();

  Game &saveState();
  void undo(bool all);
  void redo();

  bool canUndo() const;
  bool canRedo() const;

#if defined(_DEBUG)
  void printState(const TCHAR *method) const;
#endif
};
