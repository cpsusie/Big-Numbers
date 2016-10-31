#pragma once

class GameEditHistory {
private:
  Game                  m_game;
  GameKey               m_key0;
  CompactArray<GameKey> m_history;
  int                   m_index; // Invariant: 0 <= m_index <= m_history.size()

  void resetHistory();

  int getHistorySize() const {
    return (int)m_history.size();
  }

  void removeLast();
  void addKey();
  bool canAddKey() const;
  const GameKey &getHistoryKey(int i) const;
  Game &setGame(const Game &game);

#ifdef _DEBUG
  void checkInvariant(int line) const;
  void printState(const TCHAR *function) const;
#endif

public:
  GameEditHistory();

  Game &beginEdit(const Game &game);
  void endEdit();

  Game &getGame() {
    return m_game;
  }

  bool isModified();

  Game &saveState();
  void undo(bool all);
  void redo();

  bool canUndo() const;
  bool canRedo() const;
};
