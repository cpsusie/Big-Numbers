#include "stdafx.h"
#include <HashMap.h>
#include <ByteFile.h>
#include <ByteMemoryStream.h>
#include <CompressFilter.h>
#include <Random.h>
#include <Packer.h>
#include "OpeningLibrary.h"

Packer &operator<<(Packer &p, const LibraryMove &m) {
  return p << m.m_from << m.m_to << m.m_goodMove;
}

Packer &operator>>(Packer &p,       LibraryMove &m) {
  return p >> m.m_from >> m.m_to >> m.m_goodMove;
}

String LibraryMove::toString() const {
  return format(_T("(%s-%s%s)")
                ,getFieldName(m_from)
                ,getFieldName(m_to)
                ,m_goodMove?_T(""):_T("?"));
}

Packer &operator<<(Packer &p, const LibraryTransition &t) {
  return p << (LibraryMove&)t << t.m_nameIndex << t.m_nextState;
}

Packer &operator>>(Packer &p,       LibraryTransition &t) {
  return p >> (LibraryMove&)t >> t.m_nameIndex >> t.m_nextState;
}

String LibraryTransition::toString() const {
  String result = LibraryMove::toString() + _T("->") + format(_T("%-4d. <"),m_nextState);
  const TCHAR *delimiter = _T("");
  for(size_t i = 0; i < m_nameIndex.size(); i++, delimiter = _T(",")) {
    result += format(_T("%s%d"), delimiter, m_nameIndex[i]);
  }
  result += _T(">");
  return result;
}

Packer &operator<<(Packer &p, const LibraryState &s) {
  const int player = s.m_playerInTurn;
  return p << s.m_id << player << s.m_transitionArray;
}

Packer &operator>>(Packer &p, LibraryState &s) {
  int player;
  p >> s.m_id >> player >> s.m_transitionArray;
  s.m_playerInTurn = (Player)player;
  return p;
}

void LibraryState::checkPlayer(int line, Player player) const {
  if(player != m_playerInTurn) {
    throwException(_T("OpeningLibrary line %d:Player mismatch: player=%s. state.player=%s")
                  ,line
                  ,getPlayerNameEnglish(player)
                  ,getPlayerNameEnglish(m_playerInTurn));
  }
}

void LibraryState::checkAnnotation(int line, const ExecutableMove &m, const LibraryTransition &t) const {
  const bool goodMove = m.isGoodMove();
  if(goodMove != t.m_goodMove) {
    throwException(_T("OpeningLibrary line %d:Annotation mismatch in state %d. state.m_goodMove=%s, Move=%s")
                   ,line
                   ,m_id
                   ,boolToStr(t.m_goodMove)
                   ,m.toString(MOVE_LONGFORMAT).cstr());
  }
}

int LibraryState::findNextState(const ExecutableMove &m, bool validateAnnotation) const {
  checkPlayer(__LINE__, m.getPlayer());
  for(size_t i = 0; i < m_transitionArray.size(); i++) {
    const LibraryTransition &t = m_transitionArray[i];
    const int from = m.getFrom();
    const int to   = m.getTo();
    if(t.m_from == from && t.m_to == to) {
      if(validateAnnotation) {
        checkAnnotation(__LINE__, m, t);
      }
      return t.m_nextState;
    }
  }
  return -1;
}

LibraryTransition *LibraryState::findTransitionByMove(const ExecutableMove &m, bool validateAnnotation) {
  checkPlayer(__LINE__, m.getPlayer());
  for(size_t i = 0; i < m_transitionArray.size(); i++) {
    LibraryTransition *t = &m_transitionArray[i];
    const int from = m.getFrom();
    const int to   = m.getTo();
    if(t->m_from == from && t->m_to == to) {
      if(validateAnnotation) {
        checkAnnotation(__LINE__, m, *t);
      }
      return t;
    }
  }
  return NULL;
}

const LibraryTransition *LibraryState::findTransitionByMove(const ExecutableMove &m) const {
  checkPlayer(__LINE__, m.getPlayer());
  for(size_t i = 0; i < m_transitionArray.size(); i++) {
    const LibraryTransition *t = &m_transitionArray[i];
    const int from = m.getFrom();
    const int to   = m.getTo();
    if(t->m_from == from && t->m_to == to) {
      return t;
    }
  }
  return NULL;
}

int LibraryState::findTransitionIndexByNameIndex(int nameIndex) const {
  for(size_t i = 0; i < m_transitionArray.size(); i++) {
    const LibraryTransition *t = &m_transitionArray[i];
    if(t->m_nameIndex.contains(nameIndex)) {
      return i;
    }
  }
  return -1;
}

ExecutableMove LibraryState::findGoodMove(const Game &game) const {
  checkPlayer(__LINE__,game.getPlayerInTurn());
  CompactIntArray goodMoves;
  for(size_t i = 0; i < m_transitionArray.size(); i++) {
    const LibraryTransition &t = m_transitionArray[i];
    if(t.m_goodMove) {
      goodMoves.add(i);
    }
  }
  randomize();
  if(goodMoves.size() == 0) {
    return ExecutableMove();
  } else {
    const LibraryTransition &t = m_transitionArray[goodMoves.select()];
    return game.generateMove(t.m_from,t.m_to);
  }
}

String LibraryState::toString() const {
  String result;
  result += format(_T("<state %d, %s>"), m_id, getPlayerNameEnglish(m_playerInTurn));
  if(m_transitionArray.size() == 0) {
    result += _T(" : No library moves available\n");
  } else {
    result += _T("\n");
    for(int i = 0; i < (int)m_transitionArray.size(); i++) {
      result += format(_T("  %3d:%s\n"), i+1, m_transitionArray[i].toString().cstr());
    }
  }
  return result;
}

OpeningLibrary::OpeningLibrary() {
  m_stateArray.add(LibraryState(0,WHITEPLAYER));
}

Packer &operator>>(Packer &p, OpeningLibrary &l) {
  return p >> l.m_nameArray >> l.m_stateArray;
}

#ifdef LIBRARY_BUILDER
Packer &operator<<(Packer &p, const OpeningLibrary &l) {
  return p << l.m_nameArray << l.m_stateArray;
}

class GameKeyWithRepeatCount : public GameKey {
  const int m_repeatCount;
  friend bool operator==(const GameKeyWithRepeatCount &key1, const GameKeyWithRepeatCount &key2);
public:
  GameKeyWithRepeatCount(const Game &g);
  unsigned long hashCode() const;
};

GameKeyWithRepeatCount::GameKeyWithRepeatCount(const Game &g) : GameKey(g.getKey()), m_repeatCount(g.getPositionRepeats()) {
}

static unsigned long gameKeyHash(const GameKeyWithRepeatCount &key) {
  return key.hashCode();
}

unsigned long GameKeyWithRepeatCount::hashCode() const {
  return GameKey::hashCode() * 3 + m_repeatCount;
}

bool operator==(const GameKeyWithRepeatCount &key1, const GameKeyWithRepeatCount &key2) {
  return (const GameKey&)key1 == (const GameKey&)key2 
      && key1.m_repeatCount == key2.m_repeatCount;
}

static int gameKeyCmp(const GameKeyWithRepeatCount &key1, const GameKeyWithRepeatCount &key2) {
  return key1 == key2 ? 0 : 1;
}

void OpeningLibrary::addGame(const String &name) {
  FILE *f = FOPEN(name,_T("r"));
  try {
    Game game;
    game.load(f);
    String fileName = FileNameSplitter(name).getFileName();
    const int currentNameIndex = m_nameArray.size();
    m_nameArray.add(fileName);

    if(game.getStartPosition() != GameKey::getStartUpPosition()) {
      throwException(_T("Game <%s> doesn't start at the standard startposition"), fileName.cstr());
    }
    static HashMap<GameKeyWithRepeatCount, int> gameKeyStateMap(gameKeyHash, gameKeyCmp);

    const GameHistory history = game.getHistory();
    int               state   = 0;

    game.newGame();

    for(int i = 0; i < history.size(); i++) {
      const ExecutableMove &m = history[i];
      LibraryTransition *t = m_stateArray[state].findTransitionByMove(m, true);
      game.executeMove(m);
      if(t != NULL) {
        t->m_nameIndex.add(currentNameIndex);
        state = t->m_nextState;
      } else {
        int nextState;
        const GameKeyWithRepeatCount key(game);
        int *stateP = gameKeyStateMap.get(key);
        LibraryTransition tr(m.getFrom(), m.getTo(), m.isGoodMove(), currentNameIndex);
        if(stateP != NULL) {
          nextState = *stateP;
        } else {
          nextState = m_stateArray.size();
          m_stateArray.add(LibraryState(nextState, GETENEMY(m.getPlayer())));
          gameKeyStateMap.put(key, nextState);
        }
        tr.m_nextState = nextState;
        m_stateArray[state].m_transitionArray.add(tr);
        state = nextState;
      }
    }
  } catch(Exception e) {
    fclose(f);
    throwException(_T("Cannot add game <%s>:%s"), name.cstr(), e.what());
  }
  fclose(f);
}

class TransitionComparator : public Comparator<LibraryTransition> {
private:
  const OpeningLibrary *m_library;
public:
  TransitionComparator(const OpeningLibrary *library) : m_library(library) {
  }
  int compare(const LibraryTransition &t1, const LibraryTransition &t2);
  AbstractComparator *clone() const {
    return new TransitionComparator(*this);
  }
};

int TransitionComparator::compare(const LibraryTransition &t1, const LibraryTransition &t2) {
  int c = t2.m_goodMove - t1.m_goodMove;
  if(c) return c;
  c = t1.m_from - t2.m_from;
  if(c) return c;
  return t1.m_to - t2.m_to;
}

void OpeningLibrary::reduceEmptyStates() {
  IntHashMap<int> conversionMap;
  int nonEmptyStateCount = 0;
  for(int state = 0; state < m_stateArray.size(); state++) {
    const LibraryState &st = m_stateArray[state];
    if(st.m_transitionArray.size()) {
      conversionMap.put(state, nonEmptyStateCount++);
    }
  }
  const int emptyWhiteToMove = nonEmptyStateCount;
  const int emptyBlackToMove = nonEmptyStateCount + 1;
  for(state = 0; state < m_stateArray.size(); state++) {
    const LibraryState &st = m_stateArray[state];
    if(st.m_transitionArray.size() == 0) {
      conversionMap.put(state, (st.m_playerInTurn == WHITEPLAYER) ? emptyWhiteToMove : emptyBlackToMove);
    }
  }

  Array<LibraryState> newStateArray;
  for(state = 0; state < m_stateArray.size(); state++) {
    const LibraryState &st = m_stateArray[state];
    if(st.m_transitionArray.size()) {
      LibraryState newState = st;
      newState.m_id = *conversionMap.get(newState.m_id);
      for(int t = 0; t < newState.m_transitionArray.size(); t++) {
        int &nextState = newState.m_transitionArray[t].m_nextState;
        nextState = *conversionMap.get(nextState);
      }
      newStateArray.add(newState);
    }
  }
  newStateArray.add(LibraryState(emptyWhiteToMove, WHITEPLAYER));
  newStateArray.add(LibraryState(emptyBlackToMove, BLACKPLAYER));
  m_stateArray = newStateArray;

  TransitionComparator comparator(this);
  for(state = 0; state < m_stateArray.size(); state++) {
    LibraryState &st = m_stateArray[state];
    if(st.m_transitionArray.size() > 1) {
      st.m_transitionArray.sort(comparator);
    }
  }
}
#endif

int OpeningLibrary::getStateIndex(const Game &game) const {
  if(game.getStartPosition() != GameKey::getStartUpPosition()) {
    return -1;
  } else {
    return getStateIndex(game.getHistory());
  }
}

int OpeningLibrary::getStateIndex(const GameHistory &history) const {
  int state = 0;
  for(size_t i = 0; i < history.size() && state >= 0; i++) {
    state = m_stateArray[state].findNextState(history[i], false);
  }
  return state;
}

CompactIntArray OpeningLibrary::getStateHistory(const Game &game) const {
  CompactIntArray result;
  if(game.getStartPosition() != GameKey::getStartUpPosition()) {
    return result;
  } else {
    const GameHistory history = game.getHistory();
    int state = 0;
    result.add(state);
    for(size_t i = 0; i < history.size(); i++) {
      state = m_stateArray[state].findNextState(history[i], false);
      if(state < 0) {
        break;
      } else {
        result.add(state);
      }
    }
    return result;
  }
}

String OpeningLibrary::toString() const {
  String result;
  for(int i = 0; i < (int)m_nameArray.size(); i++) {
    result += format(_T("%3d. %s\n"), i, m_nameArray[i].cstr());
  }
  for(size_t i = 0; i < m_stateArray.size(); i++) {
    result += m_stateArray[i].toString();
  }
  return result;
}

StringArray OpeningLibrary::getActiveOpenings(const Game &game) const {
  CompactIntArray stateArray = getStateHistory(game);
  StringArray result;
  if(stateArray.size() == 0) {
    return result;
  } else {
    const GameHistory history = game.getHistory();
    if(stateArray.size() < history.size() + 1) {
      return result;
    }
    // stateArray.size() >= 1
    const LibraryState &lastState = getState(stateArray.last());

    if(lastState.m_transitionArray.size() > 0) {
      for(size_t i = 0; i < lastState.m_transitionArray.size(); i++) {
        const LibraryTransition &t = lastState.m_transitionArray[i];
        for(size_t j = 0; j < t.m_nameIndex.size(); j++) {
          result.add(m_nameArray[t.m_nameIndex[j]]);
        }
      }
    } else { // No transitions in the last state. Use previous state and last move to get name used opening
      if(history.size() >= 1) { // => stateArray.size() >= 2
        const LibraryState       &state = getState(stateArray[stateArray.size()-2]);
        const LibraryTransition *t      = state.findTransitionByMove(history.last());
        for(size_t i = 0; i < t->m_nameIndex.size(); i++) {
          result.add(m_nameArray[t->m_nameIndex[i]]);
        }
      }
    }
    return result;
  }
}

#ifdef LIBRARY_BUILDER
void OpeningLibrary::save(const String &name) {
  Packer p;
  p << *this;
  p.write(CompressFilter(ByteOutputFile(name)));
}
#endif

void OpeningLibrary::load(int resId) {
  m_nameArray.clear();
  m_stateArray.clear();
  Packer p;
  p.read(DecompressFilter(ByteMemoryInputStream(ByteArray().loadFromResource(resId, _T("OpeningLibrary")))));
  p >> *this;
}

ExecutableMove OpeningLibrary::findLibraryMove(const Game &game, bool talking) const {
  const int stateIndex = getStateIndex(game);

  if(stateIndex < 0) {
    return ExecutableMove();
  } else {
    const LibraryState &state = m_stateArray[stateIndex];
    if(talking) {
      clearVerbose();
      String tmp;
      for(size_t i = 0; i < state.m_transitionArray.size(); i++) {
        const LibraryTransition &tr = state.m_transitionArray[i];
        tmp += format(_T("%s-%s\n"), getFieldName(tr.m_from), getFieldName(tr.m_to));
      }
      verbose(_T("%s"), tmp.cstr());
    }
    return m_stateArray[stateIndex].findGoodMove(game);
  }
}
