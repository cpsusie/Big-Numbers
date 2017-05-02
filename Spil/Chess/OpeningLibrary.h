#pragma once

#include <Packer.h>

class LibraryMove {
public:
  char m_from;
  char m_to;
  bool m_goodMove;
  LibraryMove(int from, int to, bool goodMove) : m_from(from), m_to(to), m_goodMove(goodMove) {
  }
  String toString() const;
};

class LibraryTransition : public LibraryMove {
public:
  int               m_nextState;
  CompactShortArray m_nameIndex;
  LibraryTransition() : LibraryMove(-1,-1,false) {
  }
  LibraryTransition(int from, int to, bool goodMove, int firstNameIndex) : LibraryMove(from, to, goodMove) {
    m_nameIndex.add(firstNameIndex);
  }
  String toString() const;
};

class LibraryState {
private:
  void checkPlayer(    int line, Player player) const;
  void checkAnnotation(int line, const PrintableMove &m, const LibraryTransition &t) const;
public:
  int                      m_id;
  Player                   m_playerInTurn;
  Array<LibraryTransition> m_transitionArray;
  LibraryState() {
  }
  LibraryState(int id, Player playerInTurn) : m_id(id), m_playerInTurn(playerInTurn) {
  }
  int                      findNextState(       const PrintableMove &m, bool validateAnnotation) const;
        LibraryTransition *findTransitionByMove(const PrintableMove &m, bool validateAnnotation);
  const LibraryTransition *findTransitionByMove(const PrintableMove &m) const;
  int                      findTransitionIndexByNameIndex(int nameIndex) const;

  PrintableMove findGoodMove(const Game &game) const;
  String toString() const;
};

class OpeningLibrary {
private:
  Array<LibraryState> m_stateArray;
  StringArray         m_nameArray;

  friend Packer &operator>>(Packer &p,       OpeningLibrary &l);
#ifdef LIBRARY_BUILDER
  friend Packer &operator<<(Packer &p, const OpeningLibrary &l);
#endif

  int             getStateIndex(  const Game        &game   ) const;
  int             getStateIndex(  const GameHistory &history) const;
  CompactIntArray getStateHistory(const Game        &game   ) const;

public:
  OpeningLibrary();

#ifdef LIBRARY_BUILDER
  void addGame(const String &name);
  void reduceEmptyStates();
  void save(   const String &name);
#endif

  void load(int resId);
  PrintableMove findLibraryMove(const Game &game, bool talking) const;

  bool isLoaded() const {
    return !m_nameArray.isEmpty();
  }
  bool isEmpty() const {
    return m_stateArray.size() == 0;
  }
  const LibraryState &getState(int index) const {
    return m_stateArray[index];
  }
  int nextState(int state, int trIndex) const {
    return getState(state).m_transitionArray[trIndex].m_nextState;
  }
  const String &getName(int index) const {
    return m_nameArray[index];
  }
  const StringArray &getAllNames() const {
    return m_nameArray;
  }
  String toString() const;

  StringArray getActiveOpenings(const Game &game) const;
};
