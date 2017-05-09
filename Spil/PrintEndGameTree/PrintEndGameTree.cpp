#include "stdafx.h"

static void usage() {
  _ftprintf(stderr, _T("Usage:PrintEndGameTree [-vb] file\n"));
  exit(-1);
}

static FILE *openChessFile(const String &name) {
  FILE *f = fopen(name,_T("r"));
  if(f != NULL) {
    return f;
  } else {
    FileNameSplitter info(name);
    if(info.getExtension().length() == 0) {
      info.setExtension(_T("chs"));
    }
    return FOPEN(info.getFullPath(), _T("r"));
  }
}

static void loadGame(Game &game, const String &fileName) {
  FILE *f = openChessFile(fileName);
  Game tmp;
  try {
    tmp.load(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
  game = tmp;
}

class MoveFinder {
private:
  EndGameTablebase *m_tablebase;
  void load(const Game &game);
  void unload();
  bool isLoaded() const {
    return m_tablebase != NULL;
  }
public:
  MoveFinder();
  ~MoveFinder();
  MoveResultArray getAllMoves(const Game &game);
};

MoveFinder::MoveFinder() {
  EndGameKeyDefinition::setDbPath(_T("c:\\temp\\ChessEndGames"));
  EndGameKeyDefinition::setMetric(DEPTH_TO_MATE);
  m_tablebase = NULL;
}

MoveFinder::~MoveFinder() {
  unload();
}

void MoveFinder::load(const Game &game) {
  unload();
  const PositionSignature signature = game.getPositionSignature();
  bool swap;

  m_tablebase = EndGameTablebase::getInstanceBySignature(signature, swap);
  if(m_tablebase == NULL) {
    throwException(_T("Tablebase %s not found"), signature.toString().cstr());
  }
  m_tablebase->load();
}

void MoveFinder::unload() {
  if(isLoaded()) {
    m_tablebase->unload();
    m_tablebase = NULL;
  }
}

MoveResultArray MoveFinder::getAllMoves(const Game &game) {
  bool swap;
  if(!isLoaded() || !m_tablebase->getKeyDefinition().getPositionSignature().match(game.getPositionSignature(), swap)) {
    load(game);
  }
  return m_tablebase->getAllMoves(game).sort();
}

class MoveIndexWithReplies {
private:
  int  m_sum;
  int getSum() const;
public:
  unsigned int    m_index;
  MoveBase        m_move;
  MoveResultArray m_defenceMoves;
  MoveIndexWithReplies(unsigned int index, const MoveBase &m, const MoveResultArray &a) : m_index(index), m_move(m), m_defenceMoves(a) {
    m_defenceMoves.sort();
    m_sum = getSum();
  }
  inline int sumMovesToEnd() const {
    return m_sum;
  }
  String toString() const;
};

int MoveIndexWithReplies::getSum() const {
  if(m_defenceMoves.size() == 0) {
    return 0;
  } else {
    const int maxMoves = m_defenceMoves[0].m_result.getMovesToEnd();

    int sum   = 0;
    for(size_t i = 0; i < m_defenceMoves.size(); i++) {
      const int moves = m_defenceMoves[i].m_result.getMovesToEnd();
      if(moves < maxMoves-5) {
        break;
      }
      sum += moves;
    }
    return sum;
  }
}

String MoveIndexWithReplies::toString() const {
  String result = format(_T("%2d:%s"), m_index,m_move.toString().cstr());
  for(size_t i = 0; i < m_defenceMoves.size(); i++) {
    if(i > 0) {
      result += _T("        ");
    }
    result += m_defenceMoves[i].toString();
    result += _T("\n");
  }
  result += format(_T("sum:%.2d\n"), sumMovesToEnd());
  return result;
}

static int replyCmp(const MoveIndexWithReplies &r1, const MoveIndexWithReplies &r2) {
  return r1.sumMovesToEnd() - r2.sumMovesToEnd();
/*
  const MoveResultArray &a1 = r1.m_defenceMoves;
  const MoveResultArray &a2 = r2.m_defenceMoves;
  if(a1.size() == 0) {
    return -1;
  } else if(a2.size() == 0) {
    return 1;
  }
  for(int i = 0; i < a1.size() && i < a2.size(); i++) {
    const int d = (int)(a1[i].m_result.getPliesToEnd()) - (int)(a2[i].m_result.getPliesToEnd());
    if(d) {
      return d;
    }
  }
  if(i == a1.size()) {
    return -1;
  } else if(i == a2.size()) {
    return 1;
  } else {
    return 0;
  }
*/
}

#define INDENT_LAST    0x01
#define FIRST_DONE     0x02

class GameTreeStackElement {
public:
  BYTE m_attr;
  int  m_n, m_i;
  GameTreeStackElement(BYTE attr, int n, int i) : m_attr(attr), m_n(n), m_i(i) {
  }
  GameTreeStackElement() {
    m_attr = 0;
  }
};

class GameTree {
private:
  MoveFinder                     m_moveFinder;
  Game                           m_game;
  static CompactHashSet<GameKey> positionsDone;
  static bool                    atStartOfLine;
  GameTreeStackElement           m_indentStack[100];
  int                            m_top;
  const bool                     m_backReference;
  const bool                     m_verbose;
  mutable int                    m_lastVerboseTop;
  inline void pushIndent(bool last, int n, int i) {
    m_indentStack[m_top++] = GameTreeStackElement(last ? INDENT_LAST : 0, n,i);
  }
  inline void popIndent() {
    m_top--;
  }
  inline bool isIndentEmpty() const {
    return m_top == 0;
  }
  static void newLine();
  static void _tprintf(const TCHAR *format, ...);
  void        printDoneMark();
         void indent();
  void        printTree();
  bool        printMove(const MoveWithResult &mr, PrintableMove *em = NULL); // return true if game.position is a terminal move, ie. mate of capture
  void        printMoveStack() const;
  int         findBestWinnerMove(const MoveResultArray &a);
  MoveResultArray getAllMovesInCurrentPosition() {
    return m_moveFinder.getAllMoves(m_game);
  }
  bool isCurrentPositionDone() const {
    return positionsDone.contains(m_game.getKey());
  }

public:
  GameTree(const Game &game, bool backReference, bool verbose);
};

bool GameTree::atStartOfLine = true;
CompactHashSet<GameKey> GameTree::positionsDone;

GameTree::GameTree(const Game &game, bool backReference, bool verbose) : m_backReference(backReference), m_verbose(verbose) {
  positionsDone.clear();
  m_game           = game;
  m_top            = 0;
  m_lastVerboseTop = 0;
  printTree();
}

void GameTree::printTree() {
  if(m_backReference) {
    if(isCurrentPositionDone()) {
      printDoneMark();
      return;
    } else {
      positionsDone.add(m_game.getKey());
    }
  }
  const MoveResultArray a = getAllMovesInCurrentPosition();
  size_t n = a.size();
  if(n == 0) {
    return;
  }
  const MoveWithResult &m0 = a[0];

  if(n == 1) {
    if(printMove(m0)) {
      return;
    }
    m_game.executeMove(m0);
    printTree();
    m_game.unExecuteLastMove();
  } else { // many moves
    const EndGameResult r0 = m0.m_result;
    if(r0.isWinner() && (STATUSTOWINNER(r0.getStatus()) == m_game.getPlayerInTurn())) {
      const MoveWithResult &mb = a[findBestWinnerMove(a)];
      if(printMove(mb)) {
        return;
      }
      m_game.executeMove(mb);
      printTree();
      m_game.unExecuteLastMove();
    } else if(a[1].m_result != r0) { // only one best looser move
      printMove(m0);
      m_game.executeMove(m0);
      printTree();
      m_game.unExecuteLastMove();
    } else {
      UINT n;
      for(n = 1; n < a.size(); n++) {
        if(a[n].m_result != r0) {
          break;
        }
      }
      for(UINT i = 0; i < n; i++) {
        const MoveWithResult &mi = a[i];
        const bool last = (i == n-1);
        PrintableMove em;
        printMove(mi, &em);
        m_game.executeMove(mi);

        pushIndent(last, n, i);
        if(m_verbose) {
          printMoveStack();
        }
        printTree();
        popIndent();
        m_game.unExecuteLastMove();
        if(last) {
          break;
        }
      }
    }
  }
}

#define ISCAPTURE(move) (!m_game.isPositionEmpty(move.m_to))

bool GameTree::printMove(const MoveWithResult &mr, PrintableMove *em) {
  const PrintableMove move        = m_game.generateMove(mr.getFrom(), mr.getTo(), mr.getPromoteTo());
  const bool           isTerminal = (ISCAPTURE(mr) || (mr.m_result.getPliesToEnd() == 1));
  const int            no         = (m_game.getStartPosition().getPlayerInTurn() == WHITEPLAYER) ? PLIESTOMOVES(m_game.getPlyCount()+1) : PLIESTOMOVES(m_game.getPlyCount())+1;
  bool nl;

  if(em) *em = move;

  if(m_game.getPlayerInTurn() == WHITEPLAYER) {
    if(!atStartOfLine) newLine();
    indent();
    _tprintf(_T("%3d. "), no);
    nl = false;
  } else {
    if(atStartOfLine) {
      indent();
      _tprintf(_T("%3d.   -   "), no);
    }
    _tprintf(_T(", "));
    nl = true;
  }
  _tprintf(_T("%-6s"), move.toString().cstr());
  if(isTerminal) {
    newLine();
    return true;
  } else if(nl) {
    newLine();
  }
  return false;
}

void GameTree::printDoneMark() {
//  _tprintf(_T("  %c"), 24);
//  newLine();
}

void GameTree::newLine() { // static 
  _tprintf(_T("\n"));
  atStartOfLine = true;
}

void GameTree::indent() {
  if(isIndentEmpty()) {
    return;
  }
  for(int i = 0; i < m_top; i++) {
    BYTE &attr = m_indentStack[i].m_attr;
    BYTE ch;
    if(attr & INDENT_LAST) {
      if(attr & FIRST_DONE) {
        ch = ' ';
      } else {
        ch = 192;
        attr |= FIRST_DONE;
      }
    } else {
      if(attr & FIRST_DONE) {
        ch = 179;
      } else {
        ch = 195;
        attr |= FIRST_DONE;
      }
    }
    _tprintf(_T("  %c"), ch);
  }
}

void GameTree::_tprintf(const TCHAR *format, ...) { // static 
  va_list argptr;
  va_start(argptr, format);
  _vtprintf(format, argptr);
  va_end(argptr);
  atStartOfLine = false;
}

void GameTree::printMoveStack() const {
  int i;
  for(i = 0; i < m_top; i++) {
    const GameTreeStackElement &e = m_indentStack[i];
    _ftprintf(stderr, _T("%d/%-2d "), e.m_i+1,e.m_n);
  }
  for(;i < m_lastVerboseTop; i++) {
    _ftprintf(stderr, _T("     "));
  }
  m_lastVerboseTop = m_top;
  _ftprintf(stderr, _T("\r"));
}

int GameTree::findBestWinnerMove(const MoveResultArray &a) {
  BitSet candidates(a.size());
  const int minPliesToEnd = a[0].m_result.getPliesToEnd();
  candidates.add(0);
  int candidateCount = 1;
  for(size_t i = 1; i < a.size(); i++) {
    if(a[i].m_result.getPliesToEnd() == minPliesToEnd) {
      candidates.add(i);
      candidateCount++;
    } else {
      break;
    }
  }
  if(candidateCount == 1) {
    return 0;
  } else {
    Array<MoveIndexWithReplies> ca;
    for(Iterator<size_t> it = candidates.getIterator(); it.hasNext();) {
      const int       index = (int)it.next();
      const MoveBase &m     = a[index];
      m_game.executeMove(m);
      ca.add(MoveIndexWithReplies(index, m, getAllMovesInCurrentPosition()));
      m_game.unExecuteLastMove();
    }
    ca.sort(replyCmp);
    return ca[0].m_index;
  }
}

#pragma warning(disable:4065)

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  bool   verbose       = false;
  bool   backReference = false;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'v': verbose       = true; continue;
      case 'b': backReference = true; continue;
      default: usage();
      }
      break;
    }
  }

  if(!*argv) {
    usage();
  }
  const String fileName = *(argv++);

  try {
    redirectVerbose(VERBOSENULL);
    Game game;
    loadGame(game, fileName);
    switch(game.getPositionType()) {
    case NORMAL_POSITION   :
    case DRAW_POSITION     :
      throwException(_T("This is not an endgame"));
      break;
    case TABLEBASE_POSITION:
      _tprintf(_T("Gametree for %s:[%s]\n"), fileName.cstr(), game.toFENString().cstr());
      GameTree tree(game, backReference, verbose);
      break;
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("%s\n"), e.what());
    return -1;
  }

  return 0;
}
